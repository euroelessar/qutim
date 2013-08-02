/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#include "ircconnection.h"
#include "ircaccount_p.h"
#include "ircprotocol.h"
#include "ircchannel_p.h"
#include "irccontact_p.h"
#include "ircchannelparticipant.h"
#include "ircavatar.h"
#include "ircwhoisreplieshandler.h"
#include "ircstandartctcphandler.h"
#include <QHostInfo>
#include <QTextCodec>
#include <QRegExp>
#include <QDateTime>
#include <qutim/objectgenerator.h>
#include <qutim/chatsession.h>
#include <qutim/networkproxy.h>
#include <qutim/dataforms.h>
#include <qutim/notification.h>
#include <qutim/passworddialog.h>
#include <qutim/systemintegration.h>

namespace qutim_sdk_0_3 {

namespace irc {

static QRegExp ctcpRx("^\\001(\\S+)( (.*)|)\\001");

IrcConnection::IrcConnection(IrcAccount *account, QObject *parent) :
	QObject(parent), m_hostLookupId(0)
{
	m_socket = new QSslSocket(this);
	m_socket->setProxy(NetworkProxyManager::toNetworkProxy(NetworkProxyManager::settings(account)));
	m_account = account;
	m_messagesTimer.setInterval(500);
	connect(&m_messagesTimer, SIGNAL(timeout()), SLOT(sendNextMessage()));
	connect(m_socket, SIGNAL(readyRead()), SLOT(readData()));
	connect(m_socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), SLOT(stateChanged(QAbstractSocket::SocketState)));
	connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(error(QAbstractSocket::SocketError)));
	connect(m_socket, SIGNAL(sslErrors(QList<QSslError>)), SLOT(sslErrors(QList<QSslError>)));
	connect(m_socket, SIGNAL(encrypted()), SLOT(encrypted()));
	// Register handlers
	foreach(const ObjectGenerator *gen, ObjectGenerator::module<IrcServerMessageHandler>())
		registerHandler(gen->generate<IrcServerMessageHandler>());
	foreach(const ObjectGenerator *gen, ObjectGenerator::module<IrcCtcpHandler>())
		registerCtcpHandler(gen->generate<IrcCtcpHandler>());
	registerHandler(new IrcWhoisRepliesHandler(this));

	m_cmds
		<< 432  // ERR_ERRONEUSNICKNAME
		<< 433  // ERR_NICKNAMEINUSE
		<< 001  // RPL_WELCOME
		<< 002  // RPL_YOURHOST
		<< 003  // RPL_CREATED
		<< 004  // RPL_MYINFO
		<< 005  // RPL_BOUNCE
		<< 353  // RPL_NAMREPLY
		<< 366  // RPL_ENDOFNAMES
		<< "PING"
		<< "PRIVMSG"
		<< "JOIN"
		<< "PART"
		<< "NICK"
		<< "QUIT"
		<< "ERROR"
		<< 332  // RPL_TOPIC
		<< 333  // RPL_TOPIC_INFO
		<< "KICK"
		<< "MODE"
		<< "NOTICE"
		<< 375  // RPL_MOTDSTART
		<< 372  // RPL_MOTD
		<< 376  // RPL_ENDOFMOTD
		<< 321  // RPL_LISTSTART
		<< 322  // RPL_LIST
		<< 323  // RPL_LISTEND
		<< 521  // ERR_LISTSYNTAX
		<< 263  // RPL_TRYAGAIN
		<< 301  // RPL_AWAY
		<< 305  // RPL_UNAWAY
		<< 306;  // RPL_NOWAWAY
	registerHandler(this);

	registerCtcpHandler(new IrcStandartCtcpHandler(this));
	registerCtcpHandler(IrcAvatar::instance());
}

IrcConnection::~IrcConnection()
{
}

void IrcConnection::connectToNetwork()
{
	m_currentServer = -1;
	tryConnectToNextServer();
}

void IrcConnection::registerHandler(IrcServerMessageHandler *handler)
{
	foreach (const IrcCommand &cmd, handler->cmds())
		m_handlers.insert(cmd.value(), handler);
}

void IrcConnection::registerCtcpHandler(IrcCtcpHandler *handler)
{
	foreach (const QString &cmd, handler->ctcpCmds())
		m_ctcpHandlers.insert(cmd, handler);
}

void IrcConnection::handleMessage(IrcAccount *account, const QString &name,  const QString &host,
								  const IrcCommand &cmd, const QStringList &params)
{
	Status status = account->status();
	if (status == Status::Connecting)  {
		if (cmd == 432 || cmd == 433) // ERR_ERRONEUSNICKNAME or ERR_NICKNAMEINUSE
			tryNextNick();
	}
	if (cmd == 1 || cmd == 2 || cmd == 3 || cmd == 4) { // WELCOME
		if (status == Status::Connecting) {
			account->setStatus(Status::Online);
			foreach (IrcChannel *channel, account->d->channels) {
				if (channel->d->autojoin || channel->d->reconnect) {
					channel->join();
					channel->d->reconnect = false;
				}
			}
			if (!m_nickPassword.isEmpty()) {
				send(QString("PRIVMSG NickServ :IDENTIFY %1 %2")
					 .arg(m_nicks.value(0))
					 .arg(m_nickPassword));
			}
		}
		QString msg;
		if (cmd == 4) {
			msg = tr("Server %1 (Version %2), User modes: %3, Channel modes: %4");
			for (int i = 1; i <= 4; ++i)
				msg = msg.arg(params.value(i));
		} else {
			msg = params.value(1);
		}
		account->log(msg, false, "Welcome");
	} else if (cmd == 5) { // RPL_BOUNCE
		QStringList list = params;
		list.removeFirst();
		account->log(list.join(" "), false, "Support");
	} else if (cmd == 353) { // RPL_NAMREPLY
		QString channelName = params.value(2);
		if (channelName.isEmpty())
			qDebug() << "Incorrect RPL_NAMREPLY reply";
		IrcChannel *channel = m_account->getChannel(channelName, false);
		if (channel)
			channel->handleUserList(params.value(3).split(' ', QString::SkipEmptyParts));
	} else if (cmd == "PING") {
		QString server = params.value(0);
		server = server.mid(0, server.indexOf(' '));
		if (!server.isEmpty())
			send(QString("PONG %1").arg(server));
		else
			qDebug() << "Incorrect PING request";		
	} else if (cmd == "PRIVMSG") {
		QString text = params.value(1);
		if (ctcpRx.indexIn(text) == 0) { // Is it CTCP request?
			bool handled = false;
			QString ctcpCmd = (ctcpRx.cap(1)).toUpper();
			foreach (IrcCtcpHandler *handler, m_ctcpHandlers.values(ctcpCmd)) {
				handled = true;
				handler->handleCtcpRequest(account, name, host, params.value(0), ctcpCmd, ctcpRx.cap(3));
			}
			if (!handled)
				qDebug() << "Unknown CTCP request" << ctcpCmd << "from" << name;
			return;
		}
		handleTextMessage(name, host, params.value(0), params.value(1));
	} else if (cmd == "JOIN") {
		QString channelName = params.value(0);
		// Create a new IrcChannel if we have joined a channel.
		IrcChannel *channel = account->getChannel(channelName, name == m_account->name());
		if (channel)
			channel->handleJoin(name, host);
		else
			channelIsNotJoinedError(cmd, params.value(0));
	} else if (cmd == "PART") {
		IrcChannel *channel = account->getChannel(params.value(0), false);
		if (channel)
			channel->handlePart(name, params.value(1));
		else
			channelIsNotJoinedError(cmd, params.value(0));
	} else if (cmd == "NICK") { // Someone has changed his nick
		QString newNick = params.value(0);
		if (name == account->name()) {
			QString previous = m_nick;
			m_nick = newNick;
			emit account->nameChanged(newNick, previous);
		}
		IrcContact *contact = account->getContact(name, false);
		if (contact)
			contact->d->updateNick(params.value(0));
		else
			qDebug() << "NICK message from the unknown contact" << name;
	} else if (cmd == "QUIT") {
		IrcContact *contact = account->getContact(name, false);
		if (contact) {
			emit contact->quit(params.value(0));
		} else {
			qDebug() << "QUIT message from the unknown contact" << name;
		}
	} else if (cmd == "ERROR") {
		m_account->log(params.value(0), false, "ERROR");
		NotificationRequest request(Notification::System);
		request.setObject(m_account);
		request.setText(params.value(0));
		request.send();
		m_account->setStatus(Status(Status::Offline));
	} else if (cmd == 332) { // RPL_TOPIC
		IrcChannel *channel = account->getChannel(params.value(1), false);
		if (channel)
			channel->handleTopic(params.value(2));
		else
			channelIsNotJoinedError("RPL_TOPIC", params.value(1));
	} else if (cmd == 333) { // RPL_TOPIC_INFO
		IrcChannel *channel = account->getChannel(params.value(1), false);
		if (channel)
			channel->handleTopicInfo(params.value(2), params.value(3));
		else
			channelIsNotJoinedError("RPL_TOPIC_INFO", params.value(1));
	} else if (cmd == "KICK") {
		IrcChannel *channel = account->getChannel(params.value(0), false);
		if (channel)
			channel->handleKick(params.value(1), name, params.value(2));
		else
			channelIsNotJoinedError(cmd, params.value(0));
	} else if (cmd == "MODE") {
		QString object = params.value(0);
		if (IrcChannel *channel = account->getChannel(object, false))
			channel->handleMode(name, params.value(1), params.value(2));
		else if (IrcContact *contact = account->getContact(name, false))
			contact->handleMode(name, params.value(1), params.value(2));
	} else if (cmd == "NOTICE") {
		QString text = params.value(1);
		if (ctcpRx.indexIn(text) == 0) {
			bool handled = false;
			QString ctcpCmd = ctcpRx.cap(1);
			foreach (IrcCtcpHandler *handler, m_ctcpHandlers.values(ctcpCmd)) {
				handled = true;
				handler->handleCtcpResponse(account, name, host, params.value(0), ctcpCmd, ctcpRx.cap(3));
			}
			if (!handled)
				qDebug() << "Unknown CTCP response" << ctcpCmd << "from" << name;
			return;
		}
		QString msg = QString("%1: %2").arg(name).arg(text);
		m_account->log(msg, true, "Notice");
	} else if (cmd == 375) { // RPL_MOTDSTART
		m_account->log(tr("Message of the day:"), false, "MOTD");
	} else if (cmd == 376) { // RPL_ENDOFMOTD
		m_account->log(tr("End of message of the day"), false, "MOTD");
	} else if (cmd == 372) { // RPL_MOTD
		m_account->log(params.value(1), false, "MOTD");

	} else if (cmd == 321) { // RPL_LISTSTART
		if (m_account->d->channelListForm)
			m_account->d->channelListForm.data()->listStarted();
		else
			m_account->log(tr("Start of /LIST"), m_account->isUserInputtedCommand("LIST"), "LIST");
	} else if (cmd == 322) { // RPL_LIST
		QString channel = params.value(1);
		QString users = params.value(2);
		QString topic = IrcProtocol::ircFormatToHtml(params.value(3));
		if (m_account->d->channelListForm)
			m_account->d->channelListForm.data()->addChannel(channel, users, topic);
		else
			m_account->log(QString("Channel: %1 Users: %2 Topic: %3")
						   .arg(channel)
						   .arg(users)
						   .arg(topic),
						   m_account->isUserInputtedCommand("LIST"),
						   "LIST");
	} else if (cmd == 323) { // RPL_LISTEND
		if (m_account->d->channelListForm)
			m_account->d->channelListForm.data()->listEnded();
		else
			m_account->log(tr("End of /LIST"), m_account->isUserInputtedCommand("LIST", true), "LIST");
	} else if (cmd == 521) { // ERR_LISTSYNTAX
		QString error = tr("Bad list syntax, type /QUOTE HELP LIST");
		if (m_account->d->channelListForm)
			m_account->d->channelListForm.data()->error(error);
		m_account->log(error, true, "ERROR");
	} else if (cmd == 263) { // RPL_TRYAGAIN
		QString error = tr("Server load is temporarily too heavy.\nPlease wait a while and try again.");
		if (m_account->d->channelListForm)
			m_account->d->channelListForm.data()->error(error);
		m_account->log(error, true, "ERROR");
	} else if (cmd == 301) { // RPL_AWAY
		QString nick = params.value(1);
		IrcContact *contact = account->getContact(nick, false);
		if (contact) {
			QString awayMsg = params.value(2);
			contact->setAway(awayMsg);
			if (!awayMsg.isEmpty())
				m_account->log(tr("%1 set away message \"%2\"").arg(nick).arg(awayMsg),
							   m_account->isUserInputtedCommand("WHOIS"),
							   "Away");
			else
				m_account->log(tr("%1 removed away message").arg(nick),
							   m_account->isUserInputtedCommand("WHOIS"),
							   "Away");
		}
	} else if (cmd == 305) { // RPL_UNAWAY
		m_account->log(tr("You are no longer marked as being away"), false, "Away");
	} else if (cmd == 306) { // RPL_NOWAWAY
		m_account->log(tr("You have been marked as being away"), false, "Away");
	}
}

void IrcConnection::send(QString command, bool highPriority)
{
	if (!command.isEmpty()) {
		if (highPriority)
			m_messagesQueue.push_back(command);
		else
			m_lowPriorityMessagesQueue.push_back(command);
		if (!m_messagesTimer.isActive())
			m_messagesTimer.start();
		sendNextMessage();
	}
}

void IrcConnection::sendCtcpRequest(const QString &contact, const QString &cmd, const QString &params, bool highPriority)
{
	QString ctcp(cmd);
	if (!params.isEmpty())
		ctcp += " " + params;
	send(QString("PRIVMSG %1 :\001%2\001").arg(contact).arg(ctcp), highPriority);
}

void IrcConnection::sendCtcpReply(const QString &contact, const QString &cmd, const QString &params, bool highPriority)
{
	QString ctcp(cmd);
	if (!params.isEmpty())
		ctcp += " " + params;
	send(QString("NOTICE %1 :\001%2\001").arg(contact).arg(ctcp), highPriority);
}

void IrcConnection::disconnectFromHost(bool force)
{
	if (m_hostLookupId) {
		QHostInfo::abortHostLookup(m_hostLookupId);
		m_hostLookupId = 0;
	}
	if (m_socket->state() != QTcpSocket::UnconnectedState) {
		if (!force && m_socket->state() == QTcpSocket::ConnectedState)
			send(QString("QUIT :%1").arg("qutIM: IRC plugin"));
		m_socket->disconnectFromHost();
	}
	foreach (IrcChannel *channel, m_account->d->channels) {
		if (channel->isJoined()) {
			channel->leave(true);
			channel->d->reconnect = true;
		}
	}
}

QTcpSocket *IrcConnection::socket()
{
	return m_socket;
};

void IrcConnection::loadSettings()
{
	m_servers.clear();
	m_currentServer = 0;
	Config cfg = m_account->config();
	// List of servers.
	cfg.beginArray("servers");
	for (int i = 0; i < cfg.arraySize(); ++i) {
		cfg.setArrayIndex(i);
		IrcServer server;
		server.hostName = cfg.value("hostName", QString());
		server.ssl = cfg.value("ssl", false);
		server.acceptNotValidCert = cfg.value("acceptNotValidCert", false);
		server.port = cfg.value("port", server.ssl ? 6667 : 6697);
		server.protectedByPassword = cfg.value("protectedByPassword", false);
		if (server.protectedByPassword)
			server.password = cfg.value("password", QString(), Config::Crypted);
		m_servers << server;
	}
	cfg.endArray();
	// User nickname
	m_nicks = cfg.value("nicks").toStringList();
	if (m_nick.isEmpty())
		m_nick = m_nicks.value(0);
	// User fullname.
	m_fullName = cfg.value("fullName").toString();
	m_nickPassword = cfg.value("nickPassword").toString();
	m_codec = QTextCodec::codecForName(cfg.value("codec", "utf8").toLatin1());
	if (!m_codec)
		m_codec = QTextCodec::codecForName("utf8");
	Q_ASSERT(m_codec);
#ifndef QUTIM_MOBILE_UI
	m_autoRequestWhois = cfg.value("autoRequestWhois", true);
#else
	m_autoRequestWhois = cfg.value("autoRequestWhois", false);
#endif
}

void IrcConnection::tryConnectToNextServer()
{
	Q_ASSERT(m_hostLookupId == 0);
	QString error;
	if (m_servers.isEmpty())
		error = tr("Add at least one server before connecting");
	if (m_nicks.isEmpty())
		error = tr("Set at least one nick before connecting");
	if (++m_currentServer >= m_servers.size())
		error = tr("Cannot connect to %1 network").arg(m_account->name());
	if (!error.isEmpty()) {
		qDebug() << error;
		m_account->setStatus(Status::Offline);
		NotificationRequest request(Notification::System);
		request.setObject(m_account);
		request.setText(error);
		request.send();
		return;
	}
	m_currentNick = -1;
	IrcServer server = m_servers.at(m_currentServer);
	if (server.ssl) {
		if (server.acceptNotValidCert)
			m_socket->setPeerVerifyMode(QSslSocket::QueryPeer);
		else
			m_socket->setPeerVerifyMode(QSslSocket::VerifyPeer);
		m_socket->connectToHostEncrypted(server.hostName, server.port);
	} else {
		m_hostLookupId = QHostInfo::lookupHost(server.hostName, this, SLOT(hostFound(QHostInfo)));
	}
}

void IrcConnection::hostFound(const QHostInfo &host)
{
	m_hostLookupId = 0;
	if (!host.addresses().isEmpty()) {
		IrcServer server = m_servers.at(m_currentServer);
		m_socket->connectToHost(host.addresses().at(qrand() % host.addresses().size()), server.port);
	} else {
		tryConnectToNextServer();
	}
}

void IrcConnection::tryNextNick()
{
	if (m_nicks.isEmpty()) {
		qDebug() << "Set at least one nick before connecting";
		return;
	}
	if (++m_currentNick >= m_nicks.size()) {
		tryConnectToNextServer(); // TODO: Or should we stop connecting?
		return;
	}
	m_nick = m_nicks.at(m_currentNick);
	send(QString("NICK %1\nUSER %1 %2 * :%3")
		 .arg(m_nick).arg(0)
		 .arg(m_fullName.isEmpty() ? m_nick : m_fullName));
}

void IrcConnection::sendNextMessage()
{
	uint currentTime = QDateTime::currentDateTime().toTime_t();
	if (currentTime - m_lastMessageTime <= 2)
		return;

	QString command;
	if (!m_messagesQueue.isEmpty())
		command = m_messagesQueue.takeFirst();
	else if (!m_lowPriorityMessagesQueue.isEmpty())
		command = m_lowPriorityMessagesQueue.takeFirst();
	else
		return;

	QByteArray data = m_codec->fromUnicode(command) + "\r\n";
	qDebug() << ">>>>" << data.trimmed();
	m_socket->write(data);

	m_lastMessageTime = QDateTime::currentDateTime().toTime_t();
	if (m_messagesQueue.isEmpty() && m_lowPriorityMessagesQueue.isEmpty())
		m_messagesTimer.stop();
}

void IrcConnection::handleTextMessage(const QString &from, const QString &fromHost, const QString &to, const QString &text)
{
	QString plainText = IrcProtocol::ircFormatToPlainText(text);
	QString html = IrcProtocol::ircFormatToHtml(text);
	bool isPrivate = (to == m_nick);
	Message msg(plainText);
	msg.setIncoming(true);
	msg.setTime(QDateTime::currentDateTime());
	msg.setProperty("html", html);
	ChatSession *session;
	if (isPrivate) {
		IrcContact *contact = m_account->getContact(from, fromHost, true);
		msg.setChatUnit(contact);
		session = ChatLayer::instance()->getSession(contact, true);
		connect(session, SIGNAL(destroyed()), contact, SLOT(onSessionDestroyed()));
	} else {
		IrcChannel *channel = m_account->getChannel(to, false);
		if (!channel) {
			channelIsNotJoinedError("PRIVMSG", to);
			return;
		}
		session = ChatLayer::instance()->getSession(channel, true);
		msg.setChatUnit(channel);
		msg.setProperty("senderName", from);
		msg.setProperty("senderId", from);
	}
	session->appendMessage(msg);
}

void IrcConnection::channelIsNotJoinedError(const QString &cmd, const QString &channel, bool reply)
{
	QString str;
	if (reply)
		str = QString("%1 reply");
	else
		str = QString("%1 request");
	str = str.arg(cmd);
	qDebug() << str.toStdString().c_str() << "message on the channel" << channel
			<< "the account is not connected to";
}

void IrcConnection::readData()
{
	while (m_socket->canReadLine()) {
		QString msg = m_codec->toUnicode(m_socket->readLine());
		qDebug() << "<<<<" << msg.trimmed();
		static QRegExp rx("^(:([^\\s!@]+|)(\\S+|)\\s+|)(\\w+|\\d{3})(\\s+(.*)|)");
		if (rx.indexIn(msg) == 0) {
			QString params = rx.cap(6);
			QStringList paramList;
			static QRegExp paramRx("(:[^\\r\\n]*|[^\\s\\r\\n]+)");
			int pos = 0;
			forever {
				pos = paramRx.indexIn(params, pos);
				Q_ASSERT(paramRx.matchedLength());
				if (pos < 0)
					break;
				QString param = paramRx.cap(1);
				if (param.startsWith(':')) {
					paramList << param.mid(1);
					break;
				} else {
					paramList << param;
				}
				pos += paramRx.matchedLength();
			}
			QString name = rx.cap(2);
			QString host = rx.cap(3);
			if (host.startsWith('!'))
				host = host.mid(1);
			IrcCommand cmd(rx.cap(4));
			bool handled = false;
			foreach (IrcServerMessageHandler *handler, m_handlers.values(cmd)) {
				handled = true;
				handler->handleMessage(m_account, name, host, cmd, paramList);
			}
			if (!handled) {
				if (cmd.code() >= 400 && cmd.code() <= 502) { // Error
					m_account->log(paramList.last(), true, "ERROR");
				} else if ((cmd.code() >= 250 && cmd.code() <= 255) || cmd == 265 || cmd == 266) {
					paramList.removeFirst();
					m_account->log(paramList.join(" "), false, "Users");
				} else {
					paramList.removeFirst();
					m_account->log(paramList.join(" "), true, cmd);
				}
			}
		}
	}
}

void IrcConnection::stateChanged(QAbstractSocket::SocketState state)
{
    qWarning() << "New connection state:" << state;
	if (state == QAbstractSocket::ConnectedState) {
		SystemIntegration::keepAlive(m_socket);
		IrcServer server = m_servers.at(m_currentServer);
		if (server.protectedByPassword) {
			if (m_passDialog) {
				return;
			} else if (server.password.isEmpty()) {
				m_passDialog = PasswordDialog::request(m_account);
				QObject::connect(m_passDialog.data(), SIGNAL(entered(QString,bool)),
								 this, SLOT(passwordEntered(QString,bool)));
				QObject::connect(m_passDialog.data(), SIGNAL(rejected()),
								 m_passDialog.data(), SLOT(deleteLater()));
				return;
			} else {
				send(QString("PASS %1").arg(server.password));
			}
		}
		tryNextNick();
	} else if (state == QAbstractSocket::UnconnectedState) {
		if (m_passDialog)
			m_passDialog.data()->deleteLater();
		m_account->setStatus(Status::Offline);
	}
}

void IrcConnection::error(QAbstractSocket::SocketError error)
{
	qDebug() << "Connection error:" << error;
	NotificationRequest request(Notification::System);
	request.setObject(m_account);
	request.setText(tr("Network error: %1").arg(m_socket->errorString()));
	request.send();
	m_account->log(m_socket->errorString(), false, "ERROR");
}

void IrcConnection::sslErrors(const QList<QSslError> &errors)
{
	QString notification;
	foreach (const QSslError &error, errors) {
		m_account->log(error.errorString(), false, "ERROR");
		if (!notification.isNull())
			notification.append("\n");
		notification.append(error.errorString());
	}
	NotificationRequest request(Notification::System);
	request.setObject(m_account);
	request.setText(tr("SSL error: %1").arg(notification));
	request.send();
}

void IrcConnection::encrypted()
{
	m_account->log(tr("SSL handshake completed"), false, "Notice");
}

void IrcConnection::passwordEntered(const QString &password, bool remember)
{
	Q_ASSERT(sender() == m_passDialog.data());
	if (remember) {
		Config cfg = m_account->config();
		cfg.beginArray("servers");
		cfg.setArrayIndex(m_currentServer);
		cfg.setValue("password", password, Config::Crypted);
		cfg.endArray();
		m_servers[m_currentServer].password = password;
	}
	m_passDialog.data()->deleteLater();
	send(QString("PASS %1").arg(password));
	tryNextNick();
}

} } // namespace qutim_sdk_0_3::irc


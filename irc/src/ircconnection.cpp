/****************************************************************************
 *  ircconnection.cpp
 *
 *  Copyright (c) 2010 by Prokhin Alexey <alexey.prokhin@yandex.ru>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
 *****************************************************************************/

#include "ircconnection.h"
#include "ircaccount_p.h"
#include "ircprotocol.h"
#include "ircchannel.h"
#include "irccontact_p.h"
#include "ircchannelparticipant.h"
#include <QHostInfo>
#include <QTextCodec>
#include <QRegExp>
#include <QDateTime>
#include <qutim/objectgenerator.h>
#include <qutim/messagesession.h>

namespace qutim_sdk_0_3 {

namespace irc {

static QRegExp ctpcRx("^\\001(\\S+)( (.*)|)\\001");

IrcConnection::IrcConnection(IrcAccount *account, QObject *parent) :
	QObject(parent)
{
	m_socket = new QTcpSocket(this);
	m_account = account;
	connect(m_socket, SIGNAL(readyRead()), SLOT(readData()));
	connect(m_socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), SLOT(stateChanged(QAbstractSocket::SocketState)));
	connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(error(QAbstractSocket::SocketError)));
	// Register handlers
	foreach(const ObjectGenerator *gen, moduleGenerators<IrcServerMessageHandler>())
		registerHandler(gen->generate<IrcServerMessageHandler>());
	foreach(const ObjectGenerator *gen, moduleGenerators<IrcCtpcHandler>())
		registerCtpcHandler(gen->generate<IrcCtpcHandler>());

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
		<< 332  // RPL_TOPIC
		<< 333  // RPL_TOPIC_INFO
		<< "KICK"
		<< "MODE"
		<< "NOTICE"
		<< 375  // RPL_MOTDSTART
		<< 372  // RPL_MOTD
		<< 376; // RPL_ENDOFMOTD
	registerHandler(this);
	IrcAccount::registerLogMsgColor("ERROR", "red");
	IrcAccount::registerLogMsgColor("Notice", "magenta");
	IrcAccount::registerLogMsgColor("MOTD", "green");
	IrcAccount::registerLogMsgColor("Welcome", "green");
	IrcAccount::registerLogMsgColor("Support", "green");
	IrcAccount::registerLogMsgColor("Users", "green");

	m_ctpcCmds << "PING" << "PONG" << "ACTION";
	registerCtpcHandler(this);
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

void IrcConnection::registerCtpcHandler(IrcCtpcHandler *handler)
{
	foreach (const QString &cmd, handler->ctpcCmds())
		m_ctpcHandlers.insert(cmd, handler);
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
		if (status == Status::Connecting)
			account->setStatus(Status::Online);
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
			debug() << "Incorrect RPL_NAMREPLY reply";
		IrcChannel *channel = m_account->getChannel(channelName, false);
		if (channel)
			channel->handleUserList(params.value(3).split(' ', QString::SkipEmptyParts));
	} else if (cmd == "PING") {
		QString server = params.value(0);
		server = server.mid(0, server.indexOf(' '));
		if (!server.isEmpty())
			send(QString("PONG %1").arg(server));
		else
			debug() << "Incorrect PING request";		
	} else if (cmd == "PRIVMSG") {
		QString text = params.value(1);
		if (ctpcRx.indexIn(text) == 0) { // Is it CTPC request?
			bool handled = false;
			QString ctpcCmd = ctpcRx.cap(1);
			foreach (IrcCtpcHandler *handler, m_ctpcHandlers.values(ctpcCmd)) {
				handled = true;
				handler->handleCtpcRequest(account, name, host, params.value(0), ctpcCmd, ctpcRx.cap(3));
			}
			if (!handled)
				debug() << "Unknown CTPC request" << ctpcCmd << "from" << name;
			return;
		}
		handleTextMessage(name, params.value(0), params.value(1));
	} else if (cmd == "JOIN") {
		QString channelName = params.value(0);
		if (name == m_account->name()) { // We has been connected to the channel.
			IrcChannel *channel = account->getChannel(channelName, true);
			emit channel->joined();
			ChatSession *session = ChatLayer::instance()->getSession(channel, true);
			session->activate();
		} else { // Someone has joined the channel.
			IrcChannel *channel = account->getChannel(channelName, false);
			if (channel)
				channel->handleJoin(name, host);
			else
				channelIsNotJoinedError(cmd, params.value(0));
		}
	} else if (cmd == "PART") {
		IrcChannel *channel = account->getChannel(params.value(0), false);
		if (channel)
			channel->handlePart(name, params.value(1));
		else
			channelIsNotJoinedError(cmd, params.value(0));
	} else if (cmd == "NICK") { // Someone has changed his nick
		QString newNick = params.value(0);
		if (name == account->name()) {
			m_nick = newNick;
			emit account->nameChanged(newNick);
		}
		IrcContact *contact = account->getContact(name, false);
		if (contact)
			contact->d->updateNick(params.value(0));
		else
			debug() << "NICK message from the unknown contact" << name;
	} else if (cmd == "QUIT") {
		IrcContact *contact = account->getContact(name, false);
		if (contact) {
			emit contact->quit(params.value(0));
			contact->deleteLater();
		} else {
			debug() << "QUIT message from the unknown contact" << name;
		}
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
		if (ctpcRx.indexIn(text) == 0) {
			bool handled = false;
			QString ctpcCmd = ctpcRx.cap(1);
			foreach (IrcCtpcHandler *handler, m_ctpcHandlers.values(ctpcCmd)) {
				handled = true;
				handler->handleCtpcResponse(account, name, host, params.value(0), ctpcCmd, ctpcRx.cap(3));
			}
			if (!handled)
				debug() << "Unknown CTPC response" << ctpcCmd << "from" << name;
			return;
		}
		QString msg = QString("%1: %2").arg(name).arg(text);
		m_account->log(msg, true, "Notice");
	} else if (cmd == 375) { // RPL_MOTDSTART
		m_account->log(tr("Message of the day:"), false, "MOTD");
	} else if (cmd == 376) { // RPL_ENDOFMOTD
		m_account->log(tr("End of message of the day"), false, "MOTD");
	} else if (cmd == 372) { //RPL_MOTD
		m_account->log(params.value(1), false, "MOTD");
	}
}

void IrcConnection::handleCtpcRequest(IrcAccount *account, const QString &sender, const QString &senderHost,
									  const QString &receiver, const QString &cmd, const QString &params)
{
	Q_UNUSED(account);
	Q_UNUSED(senderHost);
	Q_UNUSED(receiver);
	if (cmd == "PING") {
		sendCtpcReply(sender, "PING", params);
#if 0
		QDateTime current = QDateTime::currentDateTime();
		QString timeStamp = QString("%1.%2").arg(current.toTime_t()).arg(current.time().msec());
		sendCtpcRequest(sender, "PING", timeStamp);
#endif
	} else if (cmd == "ACTION") {
		handleTextMessage(sender, receiver, QLatin1String("/me ") + params);
	}
}

void IrcConnection::handleCtpcResponse(IrcAccount *account, const QString &sender, const QString &senderHost,
									   const QString &receiver, const QString &cmd, const QString &params)
{
	Q_UNUSED(account);
	Q_UNUSED(senderHost);
	Q_UNUSED(receiver);
	if (cmd == "PING" || cmd == "PONG") {
		QDateTime current = QDateTime::currentDateTime();
		double receivedStamp = params.toDouble();
		if (receivedStamp >= 0) {
			double currentStamp = (double)current.time().msec() / 1000 + current.toTime_t();
			double diff = currentStamp - receivedStamp;
			account->log(tr("Received CTCP-PING reply from %1: %2 seconds", "", diff)
						 .arg(sender)
						 .arg(diff, 0, 'f', 3),
						 true, "CTPC");
		}
	}
}

void IrcConnection::send(const QString &command) const
{
	QByteArray data = m_codec->fromUnicode(command) + "\r\n";
	debug(VeryVerbose) << ">>>>" << data.trimmed();
	m_socket->write(data);
}

void IrcConnection::sendCtpcRequest(const QString &contact, const QString &cmd, const QString &params)
{
	QString ctpc(cmd);
	if (!params.isEmpty())
		ctpc += " " + params;
	send(QString("PRIVMSG %1 :\001%2\001").arg(contact).arg(ctpc));
}

void IrcConnection::sendCtpcReply(const QString &contact, const QString &cmd, const QString &params)
{
	QString ctpc(cmd);
	if (!params.isEmpty())
		ctpc += " " + params;
	send(QString("NOTICE %1 :\001%2\001").arg(contact).arg(ctpc));
}

void IrcConnection::disconnectFromHost(bool force)
{
	Q_UNUSED(force);
	if (force)
		m_socket->disconnectFromHost();
	else
		send(QString("QUIT :%1").arg("qutIM: IRC plugin"));
}

QTcpSocket *IrcConnection::socket()
{
	return m_socket;
};

bool IrcConnection::isConnected()
{
	return m_socket->state() != QTcpSocket::UnconnectedState;
}

void IrcConnection::loadSettings()
{
	m_servers.clear();
	m_currentServer = 0;
	Config cfg = m_account->config();
	Config protoCfg = m_account->protocol()->config();
	// List of servers.
	cfg.beginArray("servers");
	for (int i = 0; i < cfg.arraySize(); ++i) {
		cfg.setArrayIndex(i);
		IrcServer server;
		server.hostName = cfg.value("hostName", QString());
		server.port = cfg.value("port", 6667);
		server.protectedByPassword = cfg.value("protectedByPassword", false);
		if (server.protectedByPassword)
			server.password = cfg.value("password", QString(), Config::Crypted);
		m_servers << server;
	}
	cfg.endArray();
	// User nickname
	m_nicks = cfg.value("nicks").toStringList();
	if (m_nicks.isEmpty())
		m_nicks = protoCfg.value("nicks").toStringList();
	if (m_nick.isEmpty())
		m_nick = m_nicks.value(0);
	// User fullname.
	m_fullName = cfg.value("fullName").toString();
	if (m_fullName.isEmpty())
		m_fullName = protoCfg.value("fullName").toString();
	m_codec = QTextCodec::codecForName(cfg.value("codec", "utf8").toLatin1());
	if (!m_codec)
		m_codec = QTextCodec::codecForName("utf8");
	Q_ASSERT(m_codec);
}

void IrcConnection::tryConnectToNextServer()
{
	QString error;
	if (m_servers.isEmpty())
		error = "Add at least one server before connecting";
	if (m_nicks.isEmpty())
		error = "Set at least one nick before connecting";
	if (++m_currentServer >= m_servers.size())
		error = QString("Cannot connect to %1 network").arg(m_account->name());
	if (!error.isEmpty()) {
		debug() << error.toStdString().c_str();
		m_account->setStatus(Status::Offline);
		return;
	}
	m_currentNick = -1;
	IrcServer server = m_servers.at(m_currentServer);
	QHostInfo host = QHostInfo::fromName(server.hostName);
	if (!host.addresses().isEmpty())
		m_socket->connectToHost(host.addresses().at(qrand() % host.addresses().size()), server.port);
	else
		tryConnectToNextServer();
}

void IrcConnection::tryNextNick()
{
	if (m_nicks.isEmpty()) {
		debug() << "Set at least one nick before connecting";
		return;
	}
	if (++m_currentNick >= m_nicks.size())
		tryConnectToNextServer(); // TODO: Or should we stop connecting?
	m_nick = m_nicks.at(m_currentNick);
	send(QString("NICK %1").arg(m_nick));
	send(QString("USER %1 %2 * :%3").arg(m_nick).arg(0).arg(m_fullName.isEmpty() ? m_nick : m_fullName));
}

void IrcConnection::handleTextMessage(const QString &who, const QString &to, const QString &text)
{
	bool isPrivate = (to == m_nick);
	Message msg(text);
	msg.setIncoming(true);
	msg.setTime(QDateTime::currentDateTime());
	ChatSession *session;
	if (isPrivate) {
		IrcContact *contact = m_account->getContact(who, true);
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
		msg.setProperty("senderName", who);
		msg.setProperty("senderId", who);
		if (!text.contains(m_nick))
			msg.setProperty("silent", true);
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
	str.arg(cmd);
	debug() << str.toStdString().c_str() << "message on the channel" << channel
			<< "the account is not connected to";
}

void IrcConnection::readData()
{
	while (m_socket->canReadLine()) {
		QString msg = m_codec->toUnicode(m_socket->readLine());
		debug(VeryVerbose) << "<<<<" << msg.trimmed();
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
	debug(Verbose) << "New connection state:" << state;
	if (state == QAbstractSocket::ConnectedState) {
		IrcServer server = m_servers.at(m_currentServer);
		if (server.protectedByPassword) {
			send(QString("PASS %1").arg(server.password));
		} else {
			tryNextNick();
		}
	} else if (state == QAbstractSocket::UnconnectedState) {
		m_account->setStatus(Status::Offline);
	}
}

void IrcConnection::error(QAbstractSocket::SocketError error)
{
	debug() << "Connection error:" << error;
}

} } // namespace qutim_sdk_0_3::irc

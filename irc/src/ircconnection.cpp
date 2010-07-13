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

IrcConnection::IrcConnection(IrcAccount *account, QObject *parent) :
	QObject(parent)
{
	m_socket = new QTcpSocket(this);
	m_account = account;
	connect(m_socket, SIGNAL(readyRead()), SLOT(readData()));
	connect(m_socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), SLOT(stateChanged(QAbstractSocket::SocketState)));
	connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(error(QAbstractSocket::SocketError)));

	foreach(const ObjectGenerator *gen, moduleGenerators<IrcServerMessageHandler>())
		registerHandler(gen->generate<IrcServerMessageHandler>());

	m_cmds
		<< 432  // ERR_ERRONEUSNICKNAME
		<< 433  // ERR_NICKNAMEINUSE
		<< 001  // WELCOME
		<< 353  // RPL_NAMREPLY
		<< "PING"
		<< "PRIVMSG"
		<< "JOIN"
		<< "PART"
		<< "NICK"
		<< "QUIT"
		<< 332  // RPL_TOPIC
		<< 333  // RPL_TOPIC_INFO
		<< "KICK"
		<< "MODE";
	registerHandler(this);
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

void IrcConnection::handleMessage(class IrcAccount *account, const QString &name,  const QString &host,
								  const IrcCommand &cmd, const QStringList &params)
{
	Status status = account->status();
	if (status == Status::Connecting)  {
		if (cmd == 432 || cmd == 433) // ERR_ERRONEUSNICKNAME or ERR_NICKNAMEINUSE
			tryNextNick();
		else if (cmd == 1)// WELCOME
			account->setStatus(Status::Online);
	}
	if (cmd == 353) { // RPL_NAMREPLY
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
		static QRegExp ctpcRx("^\\001(\\S+)(.*)\\001");
		QString text = params.value(1);
		if (ctpcRx.indexIn(text) == 0) {
			debug() << "CTPC request" << ctpcRx.cap(1) << "from" << name;
			return;
		}
		QString to = params.value(0); // It can be an account's nick or a channel name.
		bool isPrivate = (to == m_account->name());
		Message msg(text);
		msg.setIncoming(true);
		msg.setTime(QDateTime::currentDateTime());
		ChatSession *session;
		if (isPrivate) {
			IrcContact *contact = account->getContact(name, true);
			msg.setChatUnit(contact);
			session = ChatLayer::instance()->getSession(contact, true);
			connect(session, SIGNAL(destroyed()), contact, SLOT(onSessionDestroyed()));
		} else {
			IrcChannel *channel = account->getChannel(to, false);
			if (!channel) {
				channelIsNotJoinedError(cmd, to);
				return;
			}
			session = ChatLayer::instance()->getSession(channel, true);
			msg.setChatUnit(channel);
			msg.setProperty("senderName", name);
			msg.setProperty("senderId", name);
			msg.setProperty("silent", true);
		}
		session->appendMessage(msg);
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
			account->d->name = newNick;
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
	}
}

void IrcConnection::send(const QString &command) const
{
	QByteArray data = m_codec->fromUnicode(command) + "\r\n";
	debug(VeryVerbose) << ">>>>" << data.trimmed();
	m_socket->write(data);
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
					ChatSession *session = m_account->activeSession();
					if (session) {
						Message msg(paramList.last());
						msg.setChatUnit(session->getUnit());
						msg.setProperty("service", true);
						msg.setTime(QDateTime::currentDateTime());
						session->appendMessage(msg);
					}
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

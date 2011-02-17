/****************************************************************************
 *  ircconnection.h
 *
 *  Copyright (c) 2011 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#ifndef IRCCONNECTION_H
#define IRCCONNECTION_H

#include "ircservermessagehandler.h"
#include "ircctpchandler.h"
#include "ircprotocol.h"
#include "ircaccount.h"
#include <QSslSocket>
#include <QTimer>

class QHostInfo;

namespace qutim_sdk_0_3 {

namespace irc {
	
struct IrcServer
{
	QString hostName;
	quint16 port;
	bool protectedByPassword;
	QString password;
	bool ssl;
};

class IrcConnection : public QObject, public IrcServerMessageHandler
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::irc::IrcServerMessageHandler)
public:
	explicit IrcConnection(IrcAccount *account, QObject *parent = 0);
	virtual ~IrcConnection();
	void connectToNetwork();
	void registerHandler(IrcServerMessageHandler *handler);
	void registerCtpcHandler(IrcCtpcHandler *handler);
	void send(QString command, bool highPriority = true);
	void sendCtpcRequest(const QString &contact, const QString &cmd, const QString &params, bool highPriority = true);
	void sendCtpcReply(const QString &contact, const QString &cmd, const QString &params, bool highPriority = true);
	void disconnectFromHost(bool force = false);
	QTcpSocket *socket();
	void loadSettings();
	void handleMessage(IrcAccount *account, const QString &name, const QString &host,
					   const IrcCommand &cmd, const QStringList &params);
	const QString &nick() const { return m_nick; }
	bool autoRequestWhois() const { return m_autoRequestWhois; }
	void handleTextMessage(const QString &from, const QString &fromHost, const QString &to, const QString &text);
	QStringList supportedCtpcTags() { return m_ctpcHandlers.keys(); }
private:
	void tryConnectToNextServer();
	void tryNextNick();
	void channelIsNotJoinedError(const QString &cmd, const QString &channel, bool reply = true);
private slots:
	void readData();
	void stateChanged(QAbstractSocket::SocketState);
	void error(QAbstractSocket::SocketError);
	void hostFound(const QHostInfo &host);
	void sendNextMessage();
	void sslErrors(const QList<QSslError> &errors);
	void encrypted();
private:
	QSslSocket *m_socket;
	QMultiMap<QString, IrcServerMessageHandler*> m_handlers;
	QMultiMap<QString, IrcCtpcHandler*> m_ctpcHandlers;
	IrcAccount *m_account;
	QList<IrcServer> m_servers;
	int m_currentServer;
	QStringList m_nicks;
	QString m_nick;
	int m_currentNick;
	QString m_fullName;
	QString m_nickPassword;
	QTextCodec *m_codec;
	int m_hostLookupId;
	QStringList m_messagesQueue;
	QStringList m_lowPriorityMessagesQueue;
	QTimer m_messagesTimer;
	uint m_lastMessageTime; // unix time when the last message had been sent
	bool m_autoRequestWhois;
};

} } // namespace qutim_sdk_0_3::irc

#endif // IRCCONNECTION_H

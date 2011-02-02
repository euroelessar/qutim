/****************************************************************************
 *  ircconnection.h
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

#ifndef IRCCONNECTION_H
#define iRCCONNECTION_H

#include "ircservermessagehandler.h"
#include "ircctpchandler.h"
#include "ircprotocol.h"
#include "ircaccount.h"
#include <QTcpSocket>
#include <QTimer>

class QHostInfo;

namespace qutim_sdk_0_3 {

namespace irc {

class IrcPingAlias : public IrcCommandAlias
{
public:
	IrcPingAlias();
	virtual QString generate(IrcCommandAlias::Type aliasType, const QStringList &params,
							 const QHash<QChar, QString> &extParams, QString *error = 0) const;
};
	
struct IrcServer
{
	QString hostName;
	quint16 port;
	bool protectedByPassword;
	QString password;
	//bool ssl;
};

class IrcConnection : public QObject, public IrcServerMessageHandler, public IrcCtpcHandler
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::irc::IrcServerMessageHandler qutim_sdk_0_3::irc::IrcCtpcHandler)
public:
	explicit IrcConnection(IrcAccount *account, QObject *parent = 0);
	virtual ~IrcConnection();
	void connectToNetwork();
	void registerHandler(IrcServerMessageHandler *handler);
	void registerCtpcHandler(IrcCtpcHandler *handler);
	void send(QString command, bool highPriority = false,
			  IrcCommandAlias::Type aliasType = IrcCommandAlias::Disabled,
			  const ExtendedParams &extParams = ExtendedParams());
	void sendCtpcRequest(const QString &contact, const QString &cmd, const QString &params, bool highPriority = false);
	void sendCtpcReply(const QString &contact, const QString &cmd, const QString &params, bool highPriority = false);
	void disconnectFromHost(bool force = false);
	QTcpSocket *socket();
	void loadSettings();
	void handleMessage(IrcAccount *account, const QString &name, const QString &host,
					   const IrcCommand &cmd, const QStringList &params);
	void handleCtpcRequest(IrcAccount *account, const QString &sender, const QString &senderHost,
						   const QString &receiver, const QString &cmd, const QString &params);
	void handleCtpcResponse(IrcAccount *account, const QString &sender, const QString &senderHost,
							const QString &receiver, const QString &cmd, const QString &params);
	const QString &nick() const { return m_nick; }
	bool isUserInputtedCommand(const QString &command, bool clearCommand = false);
	bool autoRequestWhois() const { return m_autoRequestWhois; }
	static void registerAlias(IrcCommandAlias *alias) { m_aliases.insert(alias->name(), alias); }
	static void removeAlias(const QString &name);
	static void removeAlias(IrcCommandAlias *alias);
private:
	void tryConnectToNextServer();
	void tryNextNick();
	void handleTextMessage(const QString &from, const QString &fromHost, const QString &to, const QString &text);
	void channelIsNotJoinedError(const QString &cmd, const QString &channel, bool reply = true);
	void removeOldCommands();
private slots:
	void readData();
	void stateChanged(QAbstractSocket::SocketState);
	void error(QAbstractSocket::SocketError);
	void hostFound(const QHostInfo &host);
	void sendNextMessage();
private:
	QTcpSocket *m_socket;
	QMultiMap<QString, IrcServerMessageHandler*> m_handlers;
	QMultiMap<QString, IrcCtpcHandler*> m_ctpcHandlers;
	IrcAccount *m_account;
	QList<IrcServer> m_servers;
	int m_currentServer;
	QStringList m_nicks;
	QString m_nick;
	int m_currentNick;
	QString m_fullName;
	QTextCodec *m_codec;
	int m_hostLookupId;
	struct LastCommand
	{
		uint time;
		QString cmd;
	};
	QList<LastCommand> m_lastCommands;
	QStringList m_messagesQueue;
	QStringList m_lowPriorityMessagesQueue;
	QTimer m_messagesTimer;
	uint m_lastMessageTime; // unix time when the last message had been sent
	bool m_autoRequestWhois;
	static QMultiHash<QString, IrcCommandAlias*> m_aliases;
};

} } // namespace qutim_sdk_0_3::irc

#endif // IRCCONNECTION_H

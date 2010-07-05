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
#include <QTcpSocket>

namespace qutim_sdk_0_3 {

namespace irc {
	
struct IrcServer
{
	QString hostName;
	quint16 port;
	bool protectedByPassword;
	QString password;
	//bool ssl;
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
	void send(const QString &command) const;
	void disconnectFromHost(bool force = false);
	QTcpSocket *socket();
	bool isConnected();
	void loadSettings();
	void handleMessage(class IrcAccount *account, const QString &name, const QString &host,
					   const IrcCommand &cmd, const QStringList &params);
	const QString &nick() const { return m_nick; }
private:
	void tryConnectToNextServer();
	void tryNextNick();
	void channelIsNotJoinedError(const QString &cmd, const QString &channel, bool reply = true);
private slots:
	void readData();
	void stateChanged(QAbstractSocket::SocketState);
	void error(QAbstractSocket::SocketError);
private:
	QTcpSocket *m_socket;
	QMultiMap<QString, IrcServerMessageHandler*> m_handlers;
	IrcAccount *m_account;
	QList<IrcServer> m_servers;
	int m_currentServer;
	QStringList m_nicks;
	QString m_nick;
	int m_currentNick;
	QString m_fullName;
	QTextCodec *m_codec;
};

} } // namespace qutim_sdk_0_3::irc

#endif // IRCCONNECTION_H

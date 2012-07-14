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

#ifndef IRCCONNECTION_H
#define IRCCONNECTION_H

#include "ircservermessagehandler.h"
#include "ircctcphandler.h"
#include "ircprotocol.h"
#include "ircaccount.h"
#include <QSslSocket>
#include <QTimer>

class QHostInfo;

namespace qutim_sdk_0_3 {

class PasswordDialog;

namespace irc {
	
struct IrcServer
{
	QString hostName;
	quint16 port;
	bool protectedByPassword;
	QString password;
	bool ssl;
	bool acceptNotValidCert;
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
	void registerCtcpHandler(IrcCtcpHandler *handler);
	void send(QString command, bool highPriority = true);
	void sendCtcpRequest(const QString &contact, const QString &cmd, const QString &params, bool highPriority = true);
	void sendCtcpReply(const QString &contact, const QString &cmd, const QString &params, bool highPriority = true);
	void disconnectFromHost(bool force = false);
	QTcpSocket *socket();
	void loadSettings();
	void handleMessage(IrcAccount *account, const QString &name, const QString &host,
					   const IrcCommand &cmd, const QStringList &params);
	const QString &nick() const { return m_nick; }
	bool autoRequestWhois() const { return m_autoRequestWhois; }
	void handleTextMessage(const QString &from, const QString &fromHost, const QString &to, const QString &text);
	QStringList supportedCtcpTags() { return m_ctcpHandlers.keys(); }
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
	void passwordEntered(const QString &password, bool remember);
private:
	QSslSocket *m_socket;
	QMultiMap<QString, IrcServerMessageHandler*> m_handlers;
	QMultiMap<QString, IrcCtcpHandler*> m_ctcpHandlers;
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
	QWeakPointer<PasswordDialog> m_passDialog;
};

} } // namespace qutim_sdk_0_3::irc

#endif // IRCCONNECTION_H


/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef OSCARCONNECTION_H
#define OSCARCONNECTION_H

#include "connection.h"
#include "oscarstatus.h"
#include "oscarauth.h"
#include <QPointer>

namespace qutim_sdk_0_3 {

namespace oscar {

class SNACHandler;
class SNAC;
class ProtocolNegotiation;
class IcqAccount;
class BuddyPicture;
class Roster;
class Feedbag;
class Md5Login;

class OscarConnection: public AbstractConnection
{
	Q_OBJECT
public:
	OscarConnection(IcqAccount *parent);
	void connectToLoginServer(const QString &password);
	void disconnectFromHost(bool force = false);
	void finishLogin();
	void connectToBOSS(const QString &host, quint16 port, const QByteArray &cookie);
	void sendStatus(OscarStatus status);
	void sendUserInfo(bool force = false);
	QAbstractSocket::SocketState state() const;
protected:
	void handleSNAC(AbstractConnection *conn, const SNAC &snac);
private slots:
	void onDisconnect();
	void onError(ConnectionError error);
	void md5Error(qutim_sdk_0_3::oscar::AbstractConnection::ConnectionError error);
	void accountInfoReceived(bool ok);
private:
	void setIdle(bool allow);
	void processNewConnection();
	void processCloseConnection();
	IcqAccount *m_account;
	quint16 m_status_flags;
	QByteArray m_auth_cookie;
	bool m_is_idle;
    QPointer<OscarAuth> m_auth;
//	QPointer<Md5Login> m_md5login;
};

} } // namespace qutim_sdk_0_3::oscar

#endif // OSCARCONNECTION_H


/****************************************************************************
 *  oscarconnection.h
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *                        Prokhin Alexey <alexey.prokhin@yandex.ru>
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
	QWeakPointer<OscarAuth> m_auth;
//	QPointer<Md5Login> m_md5login;
};

} } // namespace qutim_sdk_0_3::oscar

#endif // OSCARCONNECTION_H

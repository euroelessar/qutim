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
#include "metainfo.h"
#include "oscarstatus.h"

namespace qutim_sdk_0_3 {

namespace oscar {

class SNACHandler;
class SNAC;
class ProtocolNegotiation;
class IcqAccount;
class BuddyPicture;
class Roster;
class Feedbag;

class OscarConnection: public AbstractConnection
{
	Q_OBJECT
public:
	OscarConnection(IcqAccount *parent);
	void connectToLoginServer(const QString &password);
	void finishLogin();
	void connectToBOSS(const QString &host, quint16 port, const QByteArray &cookie);
	void sendStatus(OscarStatus status);
protected:
	void handleSNAC(AbstractConnection *conn, const SNAC &snac);
private slots:
	void disconnected();
	void md5Error(ConnectionError error);
	void accountInfoReceived(bool ok);
private:
	void sendUserInfo();
	void setIdle(bool allow);
	void processNewConnection();
	void processCloseConnection();
	IcqAccount *m_account;
	quint16 m_status_flags;
	QByteArray m_auth_cookie;
	bool m_is_idle;
};

} } // namespace qutim_sdk_0_3::oscar

#endif // OSCARCONNECTION_H

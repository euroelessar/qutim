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

struct ClientInfo
{
	QByteArray id_string;
	quint16 id_number;
	quint16 major_version;
	quint16 minor_version;
	quint16 lesser_version;
	quint16 build_number;
	quint32 distribution_number;
	QByteArray language;
	QByteArray country;
};

struct DirectConnectionInfo
{
	QHostAddress internal_ip;
	QHostAddress external_ip;
	quint32 port;
	quint8 dc_type;
	quint16 protocol_version;
	quint32 auth_cookie;
	quint32 web_front_port;
	quint32 client_futures;
	quint32 info_utime; // last info update time (unix time_t)
	quint32 extinfo_utime; // last ext info update time (i.e. icqphone status)
	quint32 extstatus_utime; // last ext status update time (i.e. phonebook)
};

class OscarConnection: public AbstractConnection
{
	Q_OBJECT
public:
	OscarConnection(IcqAccount *parent);
	void connectToLoginServer(const QString &password);
	inline const ClientInfo &clientInfo() const { return m_client_info; }
	inline const DirectConnectionInfo &dcInfo() const { return m_dc_info; }
	void finishLogin();
	void connectToBOSS(const QString &host, quint16 port, const QByteArray &cookie);
	MetaInfo *metaInfo() { return m_meta_info; }
	void sendStatus(OscarStatus status);
protected:
	void handleSNAC(AbstractConnection *conn, const SNAC &snac);
private slots:
	void disconnected();
	void md5Error(ConnectionError error);
private:
	void sendUserInfo();
	void setIdle(bool allow);
	void processNewConnection();
	void processCloseConnection();
	IcqAccount *m_account;
	quint16 m_status_flags;
	QByteArray m_auth_cookie;
	ClientInfo m_client_info;
	DirectConnectionInfo m_dc_info;
	bool m_is_idle;
	MetaInfo *m_meta_info;
};

} } // namespace qutim_sdk_0_3::oscar

#endif // OSCARCONNECTION_H

/****************************************************************************
 *  oscarconnection.h
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

namespace Icq
{

using namespace qutim_sdk_0_3;

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
	enum ConnectState
	{
		LoginServer, HaveBOSS, BOSS, Connected
	};
	OscarConnection(IcqAccount *parent);
	void connectToLoginServer();
	inline const ClientInfo &clientInfo() const { return m_client_info; }
	inline const DirectConnectionInfo &dcInfo() const { return m_dc_info; }
	IcqAccount *account() { return m_account; }
	const IcqAccount *account() const { return m_account; }
	void finishLogin();
	void connectToBOSS(const QString &host, quint16 port, const QByteArray &cookie);
	BuddyPicture *buddyPictureService() { return m_buddy_picture; }
	MetaInfo *metaInfo() { return m_meta_info; }
	Feedbag *feedbag() { return m_feedbag; }
	void sendStatus(Status status);
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
	BuddyPicture *m_buddy_picture;
	MetaInfo *m_meta_info;
	Feedbag *m_feedbag;
};

} // namespace Icq

#endif // OSCARCONNECTION_H

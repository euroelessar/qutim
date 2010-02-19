/****************************************************************************
 *  oscarconnection.cpp
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

#include "oscarconnection.h"
#include "util.h"
#include "md5login.h"
#include "feedbag.h"
#include "icqaccount_p.h"
#include "buddypicture.h"
#include "buddycaps.h"
#include "messages_p.h"
#include "oscarstatus_p.h"
#include <qutim/objectgenerator.h>
#include <qutim/notificationslayer.h>
#include <QHostInfo>
#include <QBuffer>
#include <QTimer>

namespace Icq
{

class ProtocolNegotiationImpl: public ProtocolNegotiation
{
public:
	ProtocolNegotiationImpl(QObject *parent = 0);
	void handleSNAC(AbstractConnection *conn, const SNAC &snac);
};

ProtocolNegotiationImpl::ProtocolNegotiationImpl(QObject *parent) :
	ProtocolNegotiation(parent)
{
	m_infos << SNACInfo(LocationFamily, LocationRightsReply)
			<< SNACInfo(BosFamily, PrivacyRightsReply);
}

void ProtocolNegotiationImpl::handleSNAC(AbstractConnection *conn, const SNAC &sn)
{
	ProtocolNegotiation::handleSNAC(conn, sn);
	sn.resetState();
	switch ((sn.family() << 16) | sn.subtype()) {
	// Server sends rate limits information
	case ServiceFamily << 16 | ServiceServerAsksServices: {
		// Requesting Location rights
		SNAC snac(LocationFamily, LocationCliReqRights);
		conn->send(snac);

		// Sending CLI_REQBOS
		snac.reset(BosFamily, PrivacyReqRights);
		conn->send(snac);
		break;
	}
	// Server replies via location service limitations
	case LocationFamily << 16 | LocationRightsReply: {
		// TODO: Implement, it's important
		break;
	}
	// Server replies via BLM service limitations
	case BuddyFamily << 16 | UserSrvReplyBuddy: {
		break;
	}
	// Server sends PRM service limitations to client
	case BosFamily << 16 | PrivacyRightsReply: {
		break;
	}
	}
}

OscarConnection::OscarConnection(IcqAccount *parent) :
	AbstractConnection(parent)
{
	connect(socket(), SIGNAL(disconnected()), this, SLOT(disconnected()));
	m_account = parent;
	{
		ClientInfo info =
		{ "ICQ Client", 266, 6, 5, 10, 104, 0x00007537, "ru", "ru" };
		m_client_info = info;
	}
	{
		DirectConnectionInfo info =
		{ QHostAddress(quint32(0)), QHostAddress(quint32(0)), 0, 0x04, 0x08, 0, 0x50, 0x03, 0, 0, 0 };
		m_dc_info = info;
	}
	m_status_flags = 0x0000;
	m_buddy_picture = new BuddyPicture(m_account, this);

	registerHandler(m_buddy_picture);
	m_meta_info = new MetaInfo(this);
	registerHandler(m_meta_info);
	registerHandler(new ProtocolNegotiationImpl(this));
	registerHandler(new MessagesHandler(m_account, this));
	m_is_idle = false;
	foreach(const ObjectGenerator *gen, moduleGenerators<SNACHandler>())
		registerHandler(gen->generate<SNACHandler>());
}

void OscarConnection::connectToLoginServer(const QString &password)
{
	setError(NoError);
	Md5Login *md5login = new Md5Login(password, this);
	connect(md5login->socket(), SIGNAL(disconnected()), md5login, SLOT(deleteLater()));
	connect(md5login, SIGNAL(error(ConnectionError)), this, SLOT(md5Error(ConnectionError)));
	md5login->login();
}

void OscarConnection::processNewConnection()
{
	AbstractConnection::processNewConnection();

	FLAP flap(0x01);
	flap.append<quint32>(0x01);
	flap.appendTLV<QByteArray>(0x0006, m_auth_cookie);
	flap.appendTLV<QByteArray>(0x0003, m_client_info.id_string);
	flap.appendTLV<quint16>(0x0017, m_client_info.major_version);
	flap.appendTLV<quint16>(0x0018, m_client_info.minor_version);
	flap.appendTLV<quint16>(0x0019, m_client_info.lesser_version);
	flap.appendTLV<quint16>(0x001a, m_client_info.build_number);
	flap.appendTLV<quint16>(0x0016, m_client_info.id_number);
	flap.appendTLV<quint32>(0x0014, m_client_info.distribution_number);
	flap.appendTLV<QByteArray>(0x000f, m_client_info.language);
	flap.appendTLV<QByteArray>(0x000e, m_client_info.country);
	// Unknown shit
	flap.appendTLV<quint8>(0x0094, 0x00);
	flap.appendTLV<quint32>(0x8003, 0x00100000);
	send(flap);
}

void OscarConnection::processCloseConnection()
{
	TLVMap tlvs = flap().read<TLVMap>();
	if (tlvs.contains(0x0009)) {
		setError(AnotherClientLogined);
	} else if (tlvs.contains(0x0008)) {
		DataUnit data(tlvs.value(0x0008));
		setError(static_cast<ConnectionError>(data.read<quint16>()));
	}
	if (error() != NoError)
		Notifications::sendNotification(errorString());
	socket()->close();
	//AbstractConnection::processCloseConnection();
}

void OscarConnection::finishLogin()
{
	sendUserInfo();
	m_is_idle = true;
	setIdle(false);
	SNAC snac(ServiceFamily, 0x02);
	// imitate ICQ 6 behaviour
	snac.append(QByteArray::fromHex(
		"0022 0001 0110 164f"
		"0001 0004 0110 164f"
		"0013 0004 0110 164f"
		"0002 0001 0110 164f"
		"0003 0001 0110 164f"
		"0015 0001 0110 164f"
		"0004 0001 0110 164f"
		"0006 0001 0110 164f"
		"0009 0001 0110 164f"
		"000a 0001 0110 164f"
		"000b 0001 0110 164f"));
	send(snac);
	m_account->setStatus(m_account->d_func()->lastStatus);
	metaInfo()->sendShortInfoRequest(this, m_account); // Requesting own information.
	emit m_account->loginFinished();
}

void OscarConnection::sendUserInfo()
{
	SNAC snac(LocationFamily, 0x04);
	TLV caps(0x05);
	foreach (const Capability &cap, m_account->capabilities())
		caps.append(cap);
	snac.append(caps);
	send(snac);
}

void OscarConnection::connectToBOSS(const QString &host, quint16 port, const QByteArray &cookie)
{
	m_auth_cookie = cookie;
	socket()->connectToHost(host, port);
}

void OscarConnection::disconnected()
{
	m_account->setStatus(OscarOffline);
}

void OscarConnection::md5Error(ConnectionError e)
{
	setError(e);
	if (e != NoError)
		emit error(e);
}

void OscarConnection::sendStatus(OscarStatus status)
{
	SNAC snac(ServiceFamily, ServiceClientSetStatus);
	snac.appendTLV<quint32>(0x06, (m_status_flags << 16) | status.subtype()); // Status mode and security flags
	snac.appendTLV<quint16>(0x08, 0x0000); // Error code
	TLV dc(0x0c); // Direct connection info
	dc.append<quint32>(externalIP().toIPv4Address()); // Real IP
	dc.append<quint32>(666); // DC Port
	dc.append<quint8>(m_dc_info.dc_type); // TCP/FLAG firewall settings
	dc.append<quint16>(m_dc_info.protocol_version); // Protocol version;
	dc.append<quint32>(qrand()); // DC auth cookie
	dc.append<quint32>(m_dc_info.web_front_port); // Web front port
	dc.append<quint32>(m_dc_info.client_futures); // client futures
	dc.append<quint32>(0x00000000); // last info update time
	dc.append<quint32>(0x00000000); // last ext info update time (i.e. icqphone status)
	dc.append<quint32>(0x00000000); // last ext status update time (i.e. phonebook)
	dc.append<quint16>(0x0000); // Unknown
	snac.append(dc);
	// Status item
	DataUnit statusNote;
	DataUnit statusData;
	statusNote.append<quint16>(0x02);
	statusData.append<quint16>(m_account->property("statusText").toString(), Util::utf8Codec());
	statusData.append<quint16>(0); // endcoding: utf8 by default
	statusNote.append<quint16>(0x400 | statusData.data().size()); // Flags + length
	statusNote.append(statusData.data());
	snac.appendTLV(0x1D, statusNote);
	snac.appendTLV<quint16>(0x1f, 0x00); // unknown
	send(snac);

	bool changedCapsList = false;
	CapsTypes types = capsTypes();
	CapsList caps = status.property<CapsList>("capabilities", CapsList());
	CapsList::const_iterator itr = caps.constBegin();
	CapsList::const_iterator endItr = caps.constEnd();
	while (itr != endItr) {
		types.remove(itr.key());
		m_account->setCapability(itr.value(), itr.key());
		changedCapsList = true;
		++itr;
	}
	foreach (const QString &type, types) {
		m_account->removeCapability(type);
		changedCapsList = true;
	}
	if (changedCapsList)
		sendUserInfo();
}

void OscarConnection::setIdle(bool allow)
{
	if (m_is_idle == allow)
		return;
	SNAC snac(ServiceFamily, 0x0011);
	snac.append<quint32>(allow ? 0x0000003C : 0x00000000);
	send(snac);
}

} // namespace Icq

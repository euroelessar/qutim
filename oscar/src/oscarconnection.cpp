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
#include "icqaccount.h"
#include "buddypicture.h"
#include "buddycaps.h"
#include "messages.h"
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
	void setMsgChannelParams(AbstractConnection *conn, quint16 chan, quint32 flags);
};

ProtocolNegotiationImpl::ProtocolNegotiationImpl(QObject *parent) :
	ProtocolNegotiation(parent)
{
	m_infos << SNACInfo(LocationFamily, LocationRightsReply)
			<< SNACInfo(MessageFamily, MessageSrvReplyIcbm)
			<< SNACInfo(BosFamily, PrivacyRightsReply);
}

void ProtocolNegotiationImpl::handleSNAC(AbstractConnection *conn, const SNAC &sn)
{
	ProtocolNegotiation::handleSNAC(conn, sn);
	sn.resetState();
	switch ((sn.family() << 16) | sn.subtype()) {
	// Server sends rate limits information
	case 0x00010007: {
		quint16 buddyFlags = 0x0002;
		OscarConnection *c = qobject_cast<OscarConnection*>(conn);
		Q_ASSERT(c);
		if (c->account()->avatarsSupport()) {
			// Requesting avatar service
			SNAC snac(ServiceFamily, ServiceClientNewService);
			snac.appendSimple<quint16>(AvatarFamily);
			conn->send(snac);
			buddyFlags |= 0x0001;
		}

		// Request server-stored information (SSI) service limitations
		SNAC snac(ListsFamily, ListsCliReqLists);
		snac.appendTLV<quint16>(0x0B, 0x000F); // mimic ICQ 6
		conn->send(snac);

		// Requesting Location rights
		snac.reset(LocationFamily, LocationCliReqRights);
		conn->send(snac);

		// Requesting client-side contactlist rights
		snac.reset(BuddyFamily, UserCliReqBuddy);
		// Query flags: 1 = Enable Avatars
		//              2 = Enable offline status message notification
		//              4 = Enable Avatars for offline contacts
		//              8 = Use reject for not authorized contacts
		snac.appendTLV<quint16>(0x05, buddyFlags); // mimic ICQ 6
		conn->send(snac);

		// Sending CLI_REQICBM
		snac.reset(MessageFamily, MessageCliReqIcbm);
		conn->send(snac);

		// Sending CLI_REQBOS
		snac.reset(BosFamily, PrivacyReqRights);
		conn->send(snac);

		// Requesting roster
		// TODO: Don't ask full roster each time, see SNAC(13,05) for it
		snac.reset(ListsFamily, ListsCliRequest);
		conn->send(snac);
		break;
	}
		// Server replies via location service limitations
	case 0x00020003: {
		// TODO: Implement, it's important
		break;
	}
		// Server replies via BLM service limitations
	case 0x00030003: {
		break;
	}
		// Server sends ICBM service parameters to client
	case 0x00040005: {
		quint32 dw_flags = 0x00000303;
		// TODO: Find description
#ifdef DBG_CAPHTML
		dw_flags |= 0x00000400;
#endif
		dw_flags |= 0x00000008; // typing notifications
		// Set message parameters for all channels (imitate ICQ 6)
		setMsgChannelParams(conn, 0x0000, dw_flags);
		break;
	}
		// Server sends PRM service limitations to client
	case 0x00090003: {
		break;
	}
	}
}

void ProtocolNegotiationImpl::setMsgChannelParams(AbstractConnection *conn, quint16 chan, quint32 flags)
{
	SNAC snac(MessageFamily, MessageCliSetParams);
	snac.appendSimple<quint16>(chan); // Channel
	snac.appendSimple<quint32>(flags); // Flags
	snac.appendSimple<quint16>(max_message_snac_size); // Max message snac size
	snac.appendSimple<quint16>(0x03E7); // Max sender warning level
	snac.appendSimple<quint16>(0x03E7); // Max receiver warning level
	snac.appendSimple<quint16>(client_rate_limit); // Minimum message interval in seconds
	snac.appendSimple<quint16>(0x0000); // Unknown
	conn->send(snac);
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

void OscarConnection::connectToLoginServer()
{
	setError(NoError);
	Md5Login *md5login = new Md5Login(this);
	connect(md5login->socket(), SIGNAL(disconnected()), md5login, SLOT(deleteLater()));
	connect(md5login, SIGNAL(error(ConnectionError)), this, SLOT(md5Error(ConnectionError)));
	md5login->login();
}

void OscarConnection::processNewConnection()
{
	AbstractConnection::processNewConnection();

	FLAP flap(0x01);
	flap.appendSimple<quint32>(0x01);
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
	TLVMap tlvs = flap().readTLVChain();
	if (tlvs.contains(0x0009)) {
		setError(AnotherClientLogined);
	} else if (tlvs.contains(0x0008)) {
		DataUnit data(tlvs.value(0x0008));
		setError(static_cast<ConnectionError>(data.readSimple<quint16>()));
	}
	if (error() != NoError)
		Notifications::sendNotification(errorString());
	//AbstractConnection::processCloseConnection();
}

void OscarConnection::finishLogin()
{
	sendUserInfo();
	m_is_idle = true;
	setIdle(false);
	SNAC snac(ServiceFamily, 0x02);
	// imitate ICQ 6 behaviour
	snac.appendData(QByteArray::fromHex(
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
	m_account->setStatus(ConnectingStop);
}

void OscarConnection::sendUserInfo()
{
	SNAC snac(LocationFamily, 0x04);
	TLV caps(0x05);
	foreach (const Capability &cap, m_account->capabilities())
		caps.appendValue(cap);
	snac.appendData(caps);
	send(snac);
}

quint16 qutimStatusToICQ(Status status)
{
	switch (status) {
	default:
	case Online:
		return 0x0000;
	case Away:
		return 0x0001;
	case DND:
		return 0x0013;
	case NA:
		return 0x0005;
	case Occupied:
		return 0x0011;
	case FreeChat:
		return 0x0020;
	case Evil:
		return 0x3000;
	case Depression:
		return 0x4000;
	case Invisible:
		return 0x0100;
	case AtHome:
		return 0x5000;
	case AtWork:
		return 0x6000;
	case OutToLunch:
		return 0x2001;
	}
}

void OscarConnection::connectToBOSS(const QString &host, quint16 port, const QByteArray &cookie)
{
	m_auth_cookie = cookie;
	socket()->connectToHost(host, port);
}

void OscarConnection::disconnected()
{
	m_account->setStatus(Offline);
}

void OscarConnection::md5Error(ConnectionError e)
{
	setError(e);
	if (e != NoError)
		emit error(e);
}

void OscarConnection::sendStatus(Status status)
{
	SNAC snac(ServiceFamily, ServiceClientSetStatus);
	snac.appendTLV<quint32>(0x06, (m_status_flags << 16) | qutimStatusToICQ(status)); // Status mode and security flags
	snac.appendTLV<quint16>(0x08, 0x0000); // Error code
	TLV dc(0x0c); // Direct connection info
	dc.appendValue<quint32>(externalIP().toIPv4Address()); // Real IP
	dc.appendValue<quint32>(666); // DC Port
	dc.appendValue<quint8>(m_dc_info.dc_type); // TCP/FLAG firewall settings
	dc.appendValue<quint16>(m_dc_info.protocol_version); // Protocol version;
	dc.appendValue<quint32>(qrand()); // DC auth cookie
	dc.appendValue<quint32>(m_dc_info.web_front_port); // Web front port
	dc.appendValue<quint32>(m_dc_info.client_futures); // client futures
	dc.appendValue<quint32>(0x00000000); // last info update time
	dc.appendValue<quint32>(0x00000000); // last ext info update time (i.e. icqphone status)
	dc.appendValue<quint32>(0x00000000); // last ext status update time (i.e. phonebook)
	dc.appendValue<quint16>(0x0000); // Unknown
	snac.appendData(dc);
	// Status item
	DataUnit statusNote;
	DataUnit statusData;
	statusNote.appendSimple<quint16>(0x02);
	statusData.appendData<quint16>(m_account->property("statusText").toString(), Util::utf8Codec());
	statusData.appendSimple<quint16>(0); // endcoding: utf8 by default
	statusNote.appendSimple<quint16>(0x400 | statusData.data().size()); // Flags + length
	statusNote.appendData(statusData.data());
	snac.appendTLV(0x1D, statusNote);
	snac.appendTLV<quint16>(0x1f, 0x00); // unknown
	send(snac);
}

void OscarConnection::setIdle(bool allow)
{
	if (m_is_idle == allow)
		return;
	SNAC snac(ServiceFamily, 0x0011);
	snac.appendSimple<quint32>(allow ? 0x0000003C : 0x00000000);
	send(snac);
}

} // namespace Icq

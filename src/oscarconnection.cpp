/****************************************************************************
 *  oscarconnection.cpp
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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
#include "tlv.h"
#include "snac.h"
#include "util.h"
#include "md5login.h"
#include "protocolnegotiation.h"
#include "roster.h"
#include "icqaccount.h"
#include <qutim/objectgenerator.h>
#include <QHostInfo>
#include <QBuffer>
#include <QTimer>

quint16 generate_flap_sequence()
{
	quint32 n = qrand(), s = 0;
	for (quint32 i = n; i >>= 3; s += i);
	return ((((0 - s) ^ (quint8)n) & 7) ^ n) + 2;
}

OscarConnection::OscarConnection(IcqAccount *parent) : QObject(parent)
{
	m_account = parent;
	{
		ClientInfo info = { "ICQ Client", 0x011a, 6, 5, 10, 104, 0x00007537, "ru", "ru" };
		m_client_info = info;
	}
	{
		DirectConnectionInfo info = { QHostAddress(quint32(0)), 0, 0x04, 0x08, 0, 0x50, 0x03, 0, 0, 0 };
		m_dc_info = info;
	}
	m_status_enum = Offline;
	m_status = 0x0000;
	m_status_flags = 0x0000;
	m_socket = new QTcpSocket(this);
	connect(m_socket, SIGNAL(readyRead()), SLOT(readData()));
	connect(m_socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), SLOT(stateChanged(QAbstractSocket::SocketState)));
	connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(error(QAbstractSocket::SocketError)));
	registerHandler(new Md5Login);
	registerHandler(new ProtocolNegotiation);
	m_is_idle = false;
	foreach(const ObjectGenerator *gen, moduleGenerators<SNACHandler>())
		registerHandler(gen->generate<SNACHandler>());
}

void OscarConnection::send(FLAP &flap)
{
	flap.setSeqNum(seqNum());
	qDebug("%s", flap.toByteArray().toHex().constData());
	m_socket->write(flap.header());
	m_socket->write(flap.data());
	m_socket->flush();
}

quint32 OscarConnection::send(SNAC &snac)
{
	FLAP flap(0x02);
	quint32 id = nextId();
	snac.setId(id);
	flap.appendData(snac);
	send(flap);
	return id;
}

void OscarConnection::registerHandler(SNACHandler *handler)
{
	QList<SNACInfo> infos = handler->infos();
	foreach(const SNACInfo &info, infos)
		m_handlers.insertMulti((info.first << 16) | info.second, handler);
}

void OscarConnection::setSeqNum(quint16 seqnum)
{
	// Have a look at seqNum method to understand reasons
	m_seqnum = (seqnum > 0) ? (seqnum - 1) : 0x7fff;
}

void OscarConnection::connectToLoginServer()
{
	setSeqNum(generate_flap_sequence());
	m_id = (quint32)qrand();
//	QHostInfo host = QHostInfo::fromName("login.messaging.aol.com");
//	qDebug() << host.addresses();
//	m_socket->connectToHost(host.addresses().at(qrand() % host.addresses().size()), 5190);
	m_socket->connectToHost("205.188.251.43" /*"login.icq.com"*/, 5190);
	m_state = LoginServer;
//	SNAC snac(0x17, 0x03);
//	QByteArray data = QByteArray::fromHex("2a02a8a80134");
//	snac.setData(QByteArray::fromHex(" 008e000100000100093438313339393133390005001036342e31322e32352e36343a3531393000060100e9e56c3ecc0e29b7cf5150ef982ce97c"
//			"e065f06e1b3cb12edfb8cf9a41a9151028512b2f4f64eba28f0f9879ced776d204e0a49c5b09a75746703cec05e9cd74b8f7cdb7"
//			"fc15246579a1b6520cfa7d210faf6dd16f1d3b04d831a4abe30ff1a207d464834ec647fe7441d45101d3f3a2f67325da11a5edd2"
//			"2ae545fa0b3320355aec5108bf2fa0f40b5699a9041f86c68ac837f71b55502eebdb6f55104ec99dd40a36b9fabf5f6e2e1d962b"
//			"3245dc03ee269830df5e2a07c16e81c7529b308ab9113c840576144a371bae6e756b5bd26c52e636f3fccb6d5b4984d1d03ca3d9"
//			"7832c6c3c2a7eaea69d054a7550df76e0e73a55593fcc9b5005b0a243975e65d"));
//	data += snac;
//	QBuffer buf(&data);
//	buf.open(QIODevice::ReadOnly);
//	qDebug() << m_flap.readData(&buf);
//	qDebug() << m_flap.isFinished();
//	readData();
}

void OscarConnection::approveConnection()
{
	FLAP flap(0x01);
	flap.appendSimple<quint32>(0x00000001);
	// It's some strange unknown shit, but ICQ 6.5 sends it
	flap.appendTLV<quint32>(0x8003, 0x00100000);
	send(flap);
}

void OscarConnection::disconnectFromHost(bool force)
{
	Q_UNUSED(force);
	m_socket->disconnectFromHost();
}

void OscarConnection::connectToBOSS(const QByteArray &host, int port, const QByteArray &cookie)
{
	m_state = HaveBOSS;
	m_boss_server = host;
	m_boss_port = port;
	m_auth_cookie = cookie;
}

void OscarConnection::md5Login()
{
	SNAC snac(AuthorizationFamily, 0x0006);
	snac.appendTLV<QByteArray>(0x0001, m_account->id().toLatin1());
	send(snac);
}

void OscarConnection::processNewConnection()
{
	qDebug("0x0%d %d %s", (int)m_flap.channel(), (int)m_flap.seqNum(), m_flap.data().toHex().constData());

	if(m_state == LoginServer)
	{
		approveConnection();
		md5Login();
	}
	else if(m_state == BOSS)
	{
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
	else
		return;
}

void OscarConnection::processSnac()
{
	SNAC snac = SNAC::fromByteArray(m_flap.data());
	qDebug("SNAC: 0x%x 0x%x 0x%x", (int)snac.family(), (int)snac.subtype(), (int)snac.id());
	bool found = false;
	foreach(SNACHandler *handler, m_handlers.values((snac.family() << 16)| snac.subtype()))
	{
		found = true;
		snac.resetState();
		handler->handleSNAC(this, snac);
	}
	if(!found)
		qWarning("No handlers for SNAC %02X %02X", int(snac.family()), int(snac.subtype()));
}

void OscarConnection::processCloseConnection()
{
	qDebug("0x0%d %d %s", (int)m_flap.channel(), (int)m_flap.seqNum(), m_flap.data().toHex().constData());
	FLAP flap(0x04);
	flap.appendSimple<quint32>(0x00000001);
	send(flap);
	m_socket->disconnectFromHost();
	if(m_state == HaveBOSS)
	{
		m_state = BOSS;
		m_socket->connectToHost(m_boss_server, m_boss_port);
	}
}

void OscarConnection::readData()
{
	if(m_socket->bytesAvailable() <= 0) // Hack for windows (Qt4.6 tp1).
	{
		qDebug() << "readyRead emmited but the socket is empty";
		return;
	}
	if(m_flap.readData(m_socket))
	{
		if(m_flap.isFinished())
		{
			switch(m_flap.channel())
			{
			case 0x01:
				processNewConnection();
				break;
			case 0x02:
				processSnac();
				break;
			case 0x04:
				processCloseConnection();
				break;
			default:
				qDebug("Unknown shac channel: 0x%04X", (int)m_flap.channel());
			case 0x03:
			case 0x05:
				break;
			}
			m_flap.clear();
		}
		// Just give a chance to other parts of qutIM to do something if needed
		if(m_socket->bytesAvailable())
			QTimer::singleShot(0, this, SLOT(readData()));
	}
	else
	{
		qCritical("Strange situation at %s: %d", Q_FUNC_INFO, __LINE__);
		m_socket->close();
	}
}

void OscarConnection::finishLogin()
{
	sendUserInfo();
	m_is_idle = true;
	setIdle(false);
	sendStatus();
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
	m_state = Connected;
}

void OscarConnection::sendUserInfo()
{
	SNAC snac(LocationFamily, 0x04);
	TLV caps(0x05);

	// ICQ UTF8 Support {0946134e-4c7f-11d1-8222-444553540000}
	caps.appendValue(Capability(0x0946134e, 0x4c7f11d1, 0x82224445, 0x53540000));

	// Buddy Icon {09461346-4c7f-11d1-8222-444553540000}
	caps.appendValue(Capability(0x09461346, 0x4c7f11d1, 0x82224445, 0x53540000));

	// RTF messages {97B12751-243C-4334-AD22-D6ABF73F1492}
	//caps.appendValue(Capability(0x97B12751, 0x243C4334, 0xAD22D6AB, 0xF73F1492));

	// qutIM some shit
	caps.appendValue(Capability(0x69716d75, 0x61746769, 0x656d0000, 0x00000000));
	caps.appendValue(Capability(0x09461343, 0x4c7f11d1, 0x82224445, 0x53540000));
	
	// ICQ typing {563fc809-0b6f-41bd-9f79-422609dfa2f3}
	caps.appendValue(Capability(0x563FC809, 0x0B6F41BD, 0x9F794226, 0x09DFA2F3));

	// qutIM version info
	// TODO: Send also version of system, i.e. 0x0601 for Windows Seven
	// TODO: Integrate with SystemInfo from libqutim
	// There are 5 bytes for it in the end of caps. Also it's possible to send linux distrib name by enum
	caps.appendValue<QByteArray>("qutim");
#if defined(Q_OS_WINCE)
	caps.appendValue<quint8>('c');
#elif defined(Q_OS_WIN32)
	caps.appendValue<quint8>('w');
#elif defined(Q_OS_LINUX)
	caps.appendValue<quint8>('l');
#elif defined(Q_OS_MAC)
	caps.appendValue<quint8>('m');
#elif defined(Q_OS_SYMBIAN)
	caps.appendValue<quint8>('s');
#elif defined(Q_OS_UNIX)
	caps.appendValue<quint8>('u');
#else
	caps.appendValue<quint8>('\0');
#endif
	caps.appendValue<quint8>(0x00);    // major
	caps.appendValue<quint8>(0x02);    // minor
	caps.appendValue<quint8>(0x60);    // fix
	caps.appendValue<quint16>(0x0175); // build
	caps.appendValue<quint32>(0x00000000);
	caps.appendValue<quint8>(0x00);    // 5 bytes more to 16

	// Short capability support {09460000-4c7f-11d1-8222-444553540000}
	caps.appendValue(Capability(0x09460000, 0x4c7f11d1, 0x82224445, 0x53540000));

	snac.appendData(caps);
	send(snac);
}

quint16 qutimStatusToICQ(Status status)
{
	switch(status)
	{
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
		return 0x0002;
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

void OscarConnection::setStatus(Status status)
{
	if(status < Online || status > OnThePhone || m_status_enum == status)
		return;
	m_status_enum = status;
	m_status = qutimStatusToICQ(status);
	if(m_state == Connected)
		sendStatus();
}

void OscarConnection::sendStatus()
{
	SNAC snac(ServiceFamily, 0x1e);
	snac.appendTLV<quint32>(0x06, (m_status_flags << 16) | m_status); // Status mode and security flags
	snac.appendTLV<quint16>(0x08, 0x0000); // Error code
	TLV dc(0x0c); // Direct connection info
	dc.appendValue<quint32>(m_ext_ip.toIPv4Address()); // Real IP
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
	snac.appendTLV<quint16>(0x1f, 0x00);
	send(snac);
}

void OscarConnection::setIdle(bool allow)
{
	if(m_is_idle == allow)
		return;
	SNAC snac(ServiceFamily, 0x0011);
	snac.appendSimple<quint32>(allow ? 0x0000003C : 0x00000000);
	send(snac);
}

void OscarConnection::stateChanged(QAbstractSocket::SocketState state)
{
	qDebug() << "New connection state" << state;
}

void OscarConnection::error(QAbstractSocket::SocketError error)
{
	qDebug() << IMPLEMENT_ME << error;
}

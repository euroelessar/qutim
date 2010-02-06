/****************************************************************************
 *  md5login.cpp
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

#include "md5login.h"
#include "icqaccount.h"
#include "qutim/notificationslayer.h"
#include <QCryptographicHash>
#include <QUrl>

namespace Icq
{

Md5LoginNegotiation::Md5LoginNegotiation(OscarConnection *conn, QObject *parent) :
	SNACHandler(parent), m_conn(conn)
{
	m_infos << SNACInfo(AuthorizationFamily, SignonLoginReply)
			<< SNACInfo(AuthorizationFamily, SignonAuthKey);
}

void Md5LoginNegotiation::handleSNAC(AbstractConnection *c, const SNAC &sn)
{
	Md5Login *conn = qobject_cast<Md5Login*>(c);
	Q_ASSERT(conn);
	if (sn.subtype() == SignonAuthKey) {
		const ClientInfo &client = m_conn->clientInfo();
		SNAC snac(AuthorizationFamily, SignonLoginRequest);
		snac.setId(qrand());
		snac.appendTLV<QByteArray>(0x0001, m_conn->account()->id().toUtf8());
		{
			quint32 length = qFromBigEndian<quint32>((uchar *) sn.data().constData());
			QByteArray key = sn.data().mid(2, length);
			QString password = m_conn->account()->config().group("general").value("passwd", QString(), Config::Crypted);
			key += QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Md5);
			key += "AOL Instant Messenger (SM)";
			snac.appendTLV(0x0025, QCryptographicHash::hash(key, QCryptographicHash::Md5));
		}
		// Flag for "new" md5 authorization
		snac.appendTLV(0x004c);
		snac.appendTLV<QByteArray>(0x0003, client.id_string);
		snac.appendTLV<quint16>(0x0017, client.major_version);
		snac.appendTLV<quint16>(0x0018, client.minor_version);
		snac.appendTLV<quint16>(0x0019, client.lesser_version);
		snac.appendTLV<quint16>(0x001a, client.build_number);
		snac.appendTLV<quint16>(0x0016, client.id_number);
		snac.appendTLV<quint32>(0x0014, client.distribution_number);
		snac.appendTLV<QByteArray>(0x000f, client.language);
		snac.appendTLV<QByteArray>(0x000e, client.country);
		// Unknown shit
		snac.appendTLV<quint8>(0x0094, 0x00);
		conn->send(snac);
	} else if (sn.subtype() == SignonLoginReply) {
		TLVMap tlvs = sn.read<TLVMap>();
		if (tlvs.contains(0x01) && tlvs.contains(0x05) && tlvs.contains(0x06)) {
			QList<QByteArray> list = tlvs.value(0x05).data().split(':');
			conn->setLoginData(list.at(0), list.size() > 1 ? atoi(list.at(1).constData()) : 5190, tlvs.value(0x06).data());
		} else {
			DataUnit data(tlvs.value(0x0008));
			static_cast<Md5Login*>(c)->setError(static_cast<AbstractConnection::ConnectionError>(data.read<quint16>()));
			Notifications::sendNotification(c->errorString());
		}
	}
}

Md5Login::Md5Login(OscarConnection *conn) :
	AbstractConnection(conn), m_conn(conn)
{
	setSeqNum( generate_flap_sequence());
}

Md5Login::~Md5Login()
{
}

void Md5Login::login()
{
	m_addr.clear();
	m_port = 0;
	m_cookie.clear();
	// Connecting to login server
	//	QHostInfo host = QHostInfo::fromName("login.messaging.aol.com");
	//	debug() << host.addresses();
	//	m_socket->connectToHost(host.addresses().at(qrand() % host.addresses().size()), 5190);
	socket()->connectToHost("205.188.251.43" /*"login.icq.com"*/, 5190);
	registerHandler(new Md5LoginNegotiation(m_conn, this));
}

void Md5Login::processNewConnection()
{
	FLAP flap(0x01);
	flap.append<quint32>(0x00000001);
	// It's some strange unknown shit, but ICQ 6.5 sends it
	flap.appendTLV<quint32>(0x8003, 0x00100000);
	send(flap);

	SNAC snac(AuthorizationFamily, 0x0006);
	snac.appendTLV<QByteArray>(0x0001, m_conn->account()->id().toLatin1());
	send(snac);
}

void Md5Login::processCloseConnection()
{
	AbstractConnection::processCloseConnection();
	if (!m_addr.isEmpty())
		m_conn->connectToBOSS(m_addr, m_port, m_cookie);
	else
		m_conn->account()->setStatus(Offline);
}

void Md5Login::setLoginData(const QString &addr, quint16 port, const QByteArray &cookie)
{
	m_addr = addr;
	m_port = port;
	m_cookie = cookie;
}

} // namespace Icq

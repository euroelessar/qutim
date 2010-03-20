/****************************************************************************
 *  oscarconnection.cpp
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

#include "oscarconnection.h"
#include "util.h"
#include "md5login.h"
#include "feedbag.h"
#include "icqaccount_p.h"
#include "buddycaps.h"
#include "messages_p.h"
#include "oscarstatus_p.h"
#include "sessiondataitem.h"
#include "metainfo_p.h"
#include <qutim/objectgenerator.h>
#include <qutim/notificationslayer.h>
#include <QHostInfo>
#include <QBuffer>
#include <QTimer>
#include <QNetworkProxy>

namespace qutim_sdk_0_3 {

namespace oscar {

OscarConnection::OscarConnection(IcqAccount *parent) :
	AbstractConnection(parent, parent)
{
	m_infos << SNACInfo(LocationFamily, LocationRightsReply)
			<< SNACInfo(BosFamily, PrivacyRightsReply);
	connect(socket(), SIGNAL(disconnected()), this, SLOT(disconnected()));
	m_account = parent;
	m_status_flags = 0x0000;
	registerHandler(this);
	m_is_idle = false;
	foreach(const ObjectGenerator *gen, moduleGenerators<SNACHandler>())
		registerHandler(gen->generate<SNACHandler>());
}

void OscarConnection::connectToLoginServer(const QString &password)
{
	setError(NoError);
	Md5Login *md5login = new Md5Login(password, account());
	connect(md5login->socket(), SIGNAL(disconnected()), md5login, SLOT(deleteLater()));
	connect(md5login, SIGNAL(error(ConnectionError)), this, SLOT(md5Error(ConnectionError)));
	md5login->login();
}

void OscarConnection::processNewConnection()
{
	AbstractConnection::processNewConnection();
	const ClientInfo &info = clientInfo();
	FLAP flap(0x01);
	flap.append<quint32>(0x01);
	flap.appendTLV<QByteArray>(0x0006, m_auth_cookie);
	flap.appendTLV<QByteArray>(0x0003, info.id_string);
	flap.appendTLV<quint16>(0x0017, info.major_version);
	flap.appendTLV<quint16>(0x0018, info.minor_version);
	flap.appendTLV<quint16>(0x0019, info.lesser_version);
	flap.appendTLV<quint16>(0x001a, info.build_number);
	flap.appendTLV<quint16>(0x0016, info.id_number);
	flap.appendTLV<quint32>(0x0014, info.distribution_number);
	flap.appendTLV<QByteArray>(0x000f, info.language);
	flap.appendTLV<QByteArray>(0x000e, info.country);
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
	emit m_account->loginFinished();
	ShortInfoMetaRequest *req = new ShortInfoMetaRequest(m_account); // Requesting own information.
	connect(req, SIGNAL(done(bool)), this, SLOT(accountInfoReceived(bool)));
	req->send();
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
#ifdef OSCAR_SSL_SUPPORT
	if (isSslEnabled()) {
		socket()->connectToHostEncrypted(host, port);
	} else
#endif
	{
		socket()->connectToHost(host, port);
	}
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

void OscarConnection::accountInfoReceived(bool ok)
{
	ShortInfoMetaRequest *req = qobject_cast<ShortInfoMetaRequest*>(sender());
	Q_ASSERT(req);
	if (ok)
		m_account->setName(req->value<QString>(Nick, m_account->id()));
	req->deleteLater();
}

void OscarConnection::sendStatus(OscarStatus status)
{
	SNAC snac(ServiceFamily, ServiceClientSetStatus);
	snac.appendTLV<quint32>(0x06, (m_status_flags << 16) | status.subtype()); // Status mode and security flags
	snac.appendTLV<quint16>(0x08, 0x0000); // Error code
	// Status item
	DataUnit statusData;
	{
		SessionDataItem statusNote(0x02, 0x04);
		QByteArray text = Util::utf8Codec()->fromUnicode(status.text());
		if (text.size() > 251)
			text.resize(251);
		statusNote.append<quint16>(text);
		statusNote.append<quint16>(0); // endcoding: utf8 by default
		statusData.append(statusNote);
	}
	snac.appendTLV(0x1D, statusData);
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

void OscarConnection::handleSNAC(AbstractConnection *conn, const SNAC &sn)
{
	Q_ASSERT(this == conn);
	AbstractConnection::handleSNAC(this, sn);
	sn.resetState();
	switch ((sn.family() << 16) | sn.subtype()) {
	// Server sends rate limits information
	case ServiceFamily << 16 | ServiceServerAsksServices: {
		// Requesting Location rights
		SNAC snac(LocationFamily, LocationCliReqRights);
		send(snac);

		// Sending CLI_REQBOS
		snac.reset(BosFamily, PrivacyReqRights);
		send(snac);
		break;
	}
	// Server replies via location service limitations
	case LocationFamily << 16 | LocationRightsReply: {
		// TODO: Implement, it's important
		break;
	}
	// Server sends PRM service limitations to client
	case BosFamily << 16 | PrivacyRightsReply: {
		break;
	}
	}
}

void OscarConnection::setIdle(bool allow)
{
	if (m_is_idle == allow)
		return;
	SNAC snac(ServiceFamily, 0x0011);
	snac.append<quint32>(allow ? 0x0000003C : 0x00000000);
	send(snac);
}

} } // namespace qutim_sdk_0_3::oscar

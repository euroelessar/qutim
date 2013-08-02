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

#include "oscarconnection.h"
#include "util.h"
#include "md5login.h"
#include "feedbag.h"
#include "icqaccount_p.h"
#include "buddycaps.h"
#include "messages_p.h"
#include "oscarstatus_p.h"
#include "sessiondataitem.h"
#include "metainfo/infometarequest.h"
#include <qutim/objectgenerator.h>
#include <qutim/notification.h>
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
	m_account = parent;
	m_status_flags = 0x0000;
	registerHandler(this);
	m_is_idle = false;
	foreach(const ObjectGenerator *gen, ObjectGenerator::module<SNACHandler>())
		registerHandler(gen->generate<SNACHandler>());

	registerInitializationSnac(LocationFamily, LocationCliReqRights);
	registerInitializationSnac(BosFamily, PrivacyReqRights);
}

void OscarConnection::connectToLoginServer(const QString &password)
{
	Q_UNUSED(password);
	setError(NoError);
	if (m_auth)
		m_auth.data()->deleteLater();
	m_auth = new OscarAuth(m_account);
//	connect(m_auth.data(), SIGNAL(disconnected()), m_auth.data(), SLOT(deleteLater()));
	connect(m_auth.data(), SIGNAL(error(qutim_sdk_0_3::oscar::AbstractConnection::ConnectionError)),
	        SLOT(md5Error(qutim_sdk_0_3::oscar::AbstractConnection::ConnectionError)));
	QTimer::singleShot(0, m_auth.data(), SLOT(login()));
//	if (m_md5login)
//		delete m_md5login;
//	m_md5login = new Md5Login(password, account());
//	connect(m_md5login, SIGNAL(disconnected()), m_md5login, SLOT(deleteLater()));
//	connect(m_md5login, SIGNAL(error(qutim_sdk_0_3::oscar::AbstractConnection::ConnectionError)),
//	this, SLOT(md5Error(qutim_sdk_0_3::oscar::AbstractConnection::ConnectionError)));
//	// Start connecting after the status has been updated.
//	QTimer::singleShot(0, m_md5login, SLOT(login()));
}

void OscarConnection::disconnectFromHost(bool force)
{
	if (m_auth) {
		m_auth.data()->deleteLater();
		m_auth.clear();
	} else {
		AbstractConnection::disconnectFromHost(force);
	}
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
	//AbstractConnection::processCloseConnection();
}

void OscarConnection::finishLogin()
{
	setState(Connected);
	sendUserInfo(true);
	m_is_idle = true;
	setIdle(false);
	SNAC snac(ServiceFamily, ServiceClientReady);
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
	m_account->finishLogin();
	if (m_account->d_func()->name.isEmpty()) {
		ShortInfoMetaRequest *req = new ShortInfoMetaRequest(m_account); // Requesting own information.
		connect(req, SIGNAL(done(bool)), this, SLOT(accountInfoReceived(bool)));
		req->send();
	}
}

void OscarConnection::sendUserInfo(bool force)
{
	Status status = m_account->status();
	if (!force && (status == Status::Offline || status == Status::Connecting))
		return;
	SNAC snac(LocationFamily, MessageFamily);
	TLV caps(0x05);
	foreach (const Capability &cap, m_account->capabilities())
		caps.append(cap);
	snac.append(caps);
	send(snac);
}

QAbstractSocket::SocketState OscarConnection::state() const
{
	if (m_auth) {
		OscarAuth::State state = m_auth.data()->state();
		if (state == OscarAuth::Invalid || state == OscarAuth::AtError)
			return QAbstractSocket::UnconnectedState;
		return QAbstractSocket::ConnectingState;
	}
//	if (m_md5login)
//		return m_md5login->socket()->state();
	return socket()->state();
}

//#define BOS_SERVER_SUPPORTS_SSL

void OscarConnection::connectToBOSS(const QString &host, quint16 port, const QByteArray &cookie)
{
	m_auth_cookie = cookie;
	Socket *s = socket();
	if (s->state() != QAbstractSocket::UnconnectedState)
		s->abort();
#if defined(OSCAR_SSL_SUPPORT)
	if (isSslEnabled()) {
		socket()->connectToHostEncrypted(host, port);
	} else
#endif
	{
		socket()->connectToHost(host, port);
	}
}

void OscarConnection::onDisconnect()
{
	Status status = m_account->status();
	if (status != Status::Offline) {
		status.setType(Status::Offline);
	
		Status::ChangeReason reason;
		switch (error()) {
		case MismatchNickOrPassword:
			reason = Status::ByAuthorizationFailed;
			break;
		case InternalError:
			reason = Status::ByFatalError;
			break;
		case NoError:
			reason = Status::ByUser;
			break;
		default:
			reason = Status::ByNetworkError;
			break;
		}
		status.setChangeReason(reason);

		m_account->setStatus(status);
	}
	AbstractConnection::onDisconnect();
}

void OscarConnection::onError(ConnectionError error)
{
	if (error != NoError &&
		!(m_account->status().type() == Status::Offline &&
		  error == SocketError &&
		  socket()->error() == QAbstractSocket::RemoteHostClosedError))
	{
		QString str = errorString();
		foreach (const QSslError &error, socket()->sslErrors()) {
			str += '\n';
			str += error.errorString();
		}

		qDebug() << str;
		Notification::send(str);
	}
	AbstractConnection::onError(error);
}

void OscarConnection::md5Error(ConnectionError e)
{
	setError(e, m_auth.data()->errorString());
//	setError(e, m_md5login->errorString());
	onDisconnect();
}

void OscarConnection::accountInfoReceived(bool ok)
{
	ShortInfoMetaRequest *req = qobject_cast<ShortInfoMetaRequest*>(sender());
	Q_ASSERT(req);
	if (ok && m_account->d_func()->name.isEmpty())
		m_account->setName(req->value<QString>(Nick, m_account->id()));
	req->deleteLater();
}

void OscarConnection::sendStatus(OscarStatus status)
{
	SNAC snac(ServiceFamily, ServiceClientSetStatus);
	snac.appendTLV<quint32>(0x06, (m_status_flags << 16) | status.flag()); // Status mode and security flags
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
	CapabilityHash caps = status.capabilities();
	CapabilityHash::const_iterator itr = caps.constBegin();
	CapabilityHash::const_iterator endItr = caps.constEnd();
	while (itr != endItr) {
		if (itr->isNull())
			continue;
		types.remove(itr.key());
		m_account->d_func()->setCapability(itr.value(), itr.key());
		changedCapsList = true;
		++itr;
	}
	foreach (const QString &type, types) {
		m_account->d_func()->removeCapability(type);
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


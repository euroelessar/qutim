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

#include "connection_p.h"
#include <QHostInfo>
#include <QBuffer>
#include <QCoreApplication>
#include <QNetworkProxy>
#include <qutim/networkproxy.h>
#include <qutim/systemintegration.h>

namespace qutim_sdk_0_3 {

namespace oscar {

ProtocolError::ProtocolError(const SNAC &snac)
{
	m_code = snac.read<qint16>();
	m_subcode = 0;
	m_tlvs = snac.read<TLVMap>();
	if (m_tlvs.contains(0x08)) {
		DataUnit data(m_tlvs.value(0x08));
		m_subcode = data.read<qint16>();
	}
}

QString ProtocolError::errorString()
{
	switch (m_code) {
	case (0x01):
		return QT_TRANSLATE_NOOP("ProtocolError", "Invalid SNAC header");
	case (0x02):
		return QT_TRANSLATE_NOOP("ProtocolError", "Server rate limit exceeded");
	case (0x03):
		return QT_TRANSLATE_NOOP("ProtocolError", "Client rate limit exceeded");
	case (0x04):
		return QT_TRANSLATE_NOOP("ProtocolError", "Recipient is not logged in");
	case (0x05):
		return QT_TRANSLATE_NOOP("ProtocolError", "Requested service unavailable");
	case (0x06):
		return QT_TRANSLATE_NOOP("ProtocolError", "Requested service not defined");
	case (0x07):
		return QT_TRANSLATE_NOOP("ProtocolError", "You sent obsolete SNAC");
	case (0x08):
		return QT_TRANSLATE_NOOP("ProtocolError", "Not supported by server");
	case (0x09):
		return QT_TRANSLATE_NOOP("ProtocolError", "Not supported by client");
	case (0x0A):
		return QT_TRANSLATE_NOOP("ProtocolError", "Refused by client");
	case (0x0B):
		return QT_TRANSLATE_NOOP("ProtocolError", "Reply too big");
	case (0x0C):
		return QT_TRANSLATE_NOOP("ProtocolError", "Responses lost");
	case (0x0D):
		return QT_TRANSLATE_NOOP("ProtocolError", "Request denied");
	case (0x0E):
		return QT_TRANSLATE_NOOP("ProtocolError", "Incorrect SNAC format");
	case (0x0F):
		return QT_TRANSLATE_NOOP("ProtocolError", "Insufficient rights");
	case (0x10):
		return QT_TRANSLATE_NOOP("ProtocolError", "In local permit/deny (recipient blocked)");
	case (0x11):
		return QT_TRANSLATE_NOOP("ProtocolError", "Sender too evil");
	case (0x12):
		return QT_TRANSLATE_NOOP("ProtocolError", "Receiver too evil");
	case (0x13):
		return QT_TRANSLATE_NOOP("ProtocolError", "User temporarily unavailable");
	case (0x14):
		return QT_TRANSLATE_NOOP("ProtocolError", "No match");
	case (0x15):
		return QT_TRANSLATE_NOOP("ProtocolError", "List overflow");
	case (0x16):
		return QT_TRANSLATE_NOOP("ProtocolError", "Request ambiguous");
	case (0x17):
		return QT_TRANSLATE_NOOP("ProtocolError", "Server queue full");
	case (0x18):
		return QT_TRANSLATE_NOOP("ProtocolError", "Not while on AOL");
	default:
		return QT_TRANSLATE_NOOP("ProtocolError", "Unknown error");
	}
}

OscarRate::OscarRate(const SNAC &sn, AbstractConnection *conn) :
	m_conn(conn)
{
	m_groupId = sn.read<quint16>();
	update(sn);
}

void OscarRate::update(const SNAC &sn)
{
	m_windowSize = sn.read<quint32>();
	m_clearLevel = sn.read<quint32>();
#if !MINIMIZE_RATE_MEMORY_USAGE
	m_alertLevel = sn.read<quint32>();
	m_limitLevel = sn.read<quint32>();
	m_disconnectLevel = sn.read<quint32>();
#else
	sn.skipData(12);
#endif
	m_currentLevel = sn.read<quint32>();
	m_maxLevel = sn.read<quint32>();
	m_lastTimeDiff = sn.read<quint32>();
#if !MINIMIZE_RATE_MEMORY_USAGE
	m_currentState = sn.read<quint8>();
#else
	sn.skipData(1);
#endif
	m_time = QDateTime::currentDateTime().addMSecs(-qint32(m_lastTimeDiff));
	m_defaultPriority = (m_clearLevel + m_maxLevel) / 2;
}

void OscarRate::send(const SNAC &snac, bool priority)
{
	QQueue<SNAC> &queue = priority ? m_highPriorityQueue : m_lowPriorityQueue;
	queue.enqueue(snac);
	if (!m_timer.isActive()) {
		sendNextPackets();
		if (!queue.isEmpty())
			m_timer.start(500, this);
	}
}

bool OscarRate::testRate(bool priority)
{
	quint32 timeDiff = getTimeDiff(QDateTime::currentDateTime());
	quint32 newLevel = (m_currentLevel * (m_windowSize - 1) + timeDiff) / m_windowSize;
	return newLevel > (priority ? m_clearLevel : m_defaultPriority);
}

void OscarRate::timerEvent(QTimerEvent *event)
{
	if (event->timerId() == m_timer.timerId())
		sendNextPackets();
}

void OscarRate::sendNextPackets()
{
	Q_ASSERT(!m_highPriorityQueue.isEmpty() || !m_lowPriorityQueue.isEmpty());
	QDateTime dateTime = QDateTime::currentDateTime();
	quint32 timeDiff = getTimeDiff(dateTime);

	quint32 newLevel;
	forever {
		bool priority = !m_highPriorityQueue.isEmpty();
		if (!priority && m_lowPriorityQueue.isEmpty()) {
			m_timer.stop();
			break;
		}

		newLevel = (m_currentLevel * (m_windowSize - 1) + timeDiff) / m_windowSize;
		if (newLevel < (priority ? m_clearLevel : m_defaultPriority))
			break;

		SNAC snac = priority ? m_highPriorityQueue.dequeue() : m_lowPriorityQueue.dequeue();
		m_lastTimeDiff = timeDiff;
		m_time = dateTime;
		timeDiff = 0;
		m_currentLevel = qMin(newLevel, m_maxLevel);
		m_conn->sendSnac(snac);
	}
}

quint32 OscarRate::getTimeDiff(const QDateTime &dateTime)
{
	if (dateTime.date() == m_time.date())
		return m_time.time().msecsTo(dateTime.time());
	else if (m_time.daysTo(dateTime) == 1)
		return 86400000 - m_time.time().msec() + dateTime.time().msec();
	else // That should never happen
		return 86400000;
}

AbstractConnection::AbstractConnection(IcqAccount *account, QObject *parent) :
	QObject(parent), d_ptr(new AbstractConnectionPrivate)
{
	Q_D(AbstractConnection);
	//	if (QSslSocket::supportsSsl())
	//		d->socket = new QSslSocket(this);
	//	else
	//		d->socket = new QTcpSocket(this);
	d->aliveTimer.setInterval(180000);
	connect(&d->aliveTimer, SIGNAL(timeout()), SLOT(sendAlivePacket()));
	d->socket = new Socket(this);
//#if OSCAR_SSL_SUPPORT
	d->socket->setProtocol(QSsl::AnyProtocol);
	d->socket->setPeerVerifyMode(QSslSocket::VerifyNone); // TODO:
//#endif
	d->account = account;
	setProxy(NetworkProxyManager::toNetworkProxy(NetworkProxyManager::settings(account)));
	connect(d->account, SIGNAL(proxyUpdated(QNetworkProxy)),
			SLOT(setProxy(QNetworkProxy)));
	connect(d->socket, SIGNAL(readyRead()), SLOT(readData()));
	connect(d->socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), SLOT(stateChanged(QAbstractSocket::SocketState)));
	connect(d->socket, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(error(QAbstractSocket::SocketError)));
	{
		ClientInfo info =
		{ "ICQ Client", 266, 20, 52, 1, 3916, 85, "en", "us" };
		d->clientInfo = info;
	}
	d->id = (quint32) qrand();
	d->error = NoError;
	m_infos << SNACInfo(ServiceFamily, ServiceServerReady)
			<< SNACInfo(ServiceFamily, ServiceServerNameInfo)
			<< SNACInfo(ServiceFamily, ServiceServerFamilies2)
			<< SNACInfo(ServiceFamily, ServiceServerAsksServices)
			<< SNACInfo(ServiceFamily, ServiceServerRateChange)
			<< SNACInfo(ServiceFamily, ServiceError);
	QList<SNACInfo> initSnacs;
	initSnacs << SNACInfo(ServiceFamily, ServiceClientFamilies)
			  << SNACInfo(ServiceFamily, ServiceClientReqRateInfo)
			  << SNACInfo(ServiceFamily, ServiceClientRateAck)
			  << SNACInfo(ServiceFamily, ServiceClientReady)
			  << SNACInfo(ServiceFamily, ServiceClientNewService);
	registerInitializationSnacs(initSnacs);
}

AbstractConnection::~AbstractConnection()
{
	Q_D(AbstractConnection);
	foreach(const OscarRate *rate, d->rates)
		delete rate;
}

void AbstractConnection::registerHandler(SNACHandler *handler)
{
	Q_D(AbstractConnection);
	QList<SNACInfo> infos = handler->infos();
	foreach(const SNACInfo &info, infos)
		d->handlers.insertMulti((info.first << 16) | info.second, handler);
}

void AbstractConnection::disconnectFromHost(bool force)
{
	Q_D(AbstractConnection);
	if (!force) {
		FLAP flap(0x04);
		flap.append<quint32>(0x00000001);
		send(flap);
	}
	d->socket->disconnectFromHost();
}

const QHostAddress &AbstractConnection::externalIP() const
{
	return d_func()->ext_ip;
}

const QList<quint16> &AbstractConnection::servicesList()
{
	return d_func()->services;
}

Socket *AbstractConnection::socket()
{
	return d_func()->socket;
}

const Socket *AbstractConnection::socket() const
{
	return d_func()->socket;
};

AbstractConnection::ConnectionError AbstractConnection::error()
{
	return d_func()->error;
};

void AbstractConnection::setProxy(const QNetworkProxy &oldProxy)
{
	QNetworkProxy proxy = oldProxy;
	proxy.setCapabilities(proxy.capabilities() &=~ QNetworkProxy::HostNameLookupCapability);
	qDebug() << Q_FUNC_INFO << proxy.type() << proxy.hostName() << proxy.port() << proxy.capabilities();
	d_func()->socket->setProxy(proxy);
}

QString AbstractConnection::errorString()
{
	Q_D(AbstractConnection);
	if (d->error == NoError)
		return QString();
	if (!d->errorStr.isEmpty())
		return d->errorStr;
	switch (d->error) {
	case InvalidNickOrPassword:
		return QCoreApplication::translate("ConnectionError", "Invalid nick or password");
	case ServiceUnaivalable:
		return QCoreApplication::translate("ConnectionError", "Service temporarily unavailable");
	case IncorrectNickOrPassword:
		return QCoreApplication::translate("ConnectionError", "Incorrect nick or password");
	case MismatchNickOrPassword:
		return QCoreApplication::translate("ConnectionError", "Mismatch nick or password");
	case InternalClientError:
		return QCoreApplication::translate("ConnectionError", "Internal client error (bad input to authorizer)");
	case InvalidAccount:
		return QCoreApplication::translate("ConnectionError", "Invalid account");
	case DeletedAccount:
		return QCoreApplication::translate("ConnectionError", "Deleted account");
	case ExpiredAccount:
		return QCoreApplication::translate("ConnectionError", "Expired account");
	case NoAccessToDatabase:
		return QCoreApplication::translate("ConnectionError", "No access to database");
	case NoAccessToResolver:
		return QCoreApplication::translate("ConnectionError", "No access to resolver");
	case InvalidDatabaseFields:
		return QCoreApplication::translate("ConnectionError", "Invalid database fields");
	case BadDatabaseStatus:
		return QCoreApplication::translate("ConnectionError", "Bad database status");
	case BadResolverStatus:
		return QCoreApplication::translate("ConnectionError", "Bad resolver status");
	case InternalError:
		return QCoreApplication::translate("ConnectionError", "Internal error");
	case ServiceOffline:
		return QCoreApplication::translate("ConnectionError", "Service temporarily offline");
	case SuspendedAccount:
		return QCoreApplication::translate("ConnectionError", "Suspended account");
	case DBSendError:
		return QCoreApplication::translate("ConnectionError", "DB send error");
	case DBLinkError:
		return QCoreApplication::translate("ConnectionError", "DB link error");
	case ReservationMapError:
		return QCoreApplication::translate("ConnectionError", "Reservation map error");
	case ReservationLinkError:
		return QCoreApplication::translate("ConnectionError", "Reservation link error");
	case ConnectionLimitExceeded :
		return QCoreApplication::translate("ConnectionError", "The users num connected from this IP has reached the maximum");
	case ConnectionLimitExceededReservation:
		return QCoreApplication::translate("ConnectionError", "The number of users connected from this IP has reached the maximum (reservation)");
	case RateLimitExceededReservation:
		return QCoreApplication::translate("ConnectionError", "Rate limit exceeded (reservation). Please try to reconnect in a few minutes");
	case UserHeavilyWarned:
		return QCoreApplication::translate("ConnectionError", "User too heavily warned");
	case ReservationTimeout:
		return QCoreApplication::translate("ConnectionError", "Reservation timeout");
	case ClientUpgradeRequired:
		return QCoreApplication::translate("ConnectionError", "You are using an older version of ICQ. Upgrade required");
	case ClientUpgradeRecommended:
		return QCoreApplication::translate("ConnectionError", "You are using an older version of ICQ. Upgrade recommended");
	case RateLimitExceeded:
		return QCoreApplication::translate("ConnectionError", "Rate limit exceeded. Please try to reconnect in a few minutes");
	case IcqNetworkError:
		return QCoreApplication::translate("ConnectionError", "Can't register on the ICQ network. Reconnect in a few minutes");
	case InvalidSecirID:
		return QCoreApplication::translate("ConnectionError", "Invalid SecurID");
	case AgeLimit:
		return QCoreApplication::translate("ConnectionError", "Account suspended because of your age (age < 13)");
	case AnotherClientLogined:
		return QCoreApplication::translate("ConnectionError", "Another client is logging with this UIN");
	case HostNotFound:
		return QCoreApplication::translate("ConnectionError", "No IP addresses were found for the host");
	case SocketError:
		return d_func()->socket->errorString();
	default:
		return QCoreApplication::translate("ConnectionError", "Unknown error");
	}
}

IcqAccount *AbstractConnection::account()
{
	return d_func()->account;
}

const IcqAccount *AbstractConnection::account() const
{
	return d_func()->account;
}

const ClientInfo &AbstractConnection::clientInfo()
{
	return d_func()->clientInfo;
}

bool AbstractConnection::isSslEnabled()
{
#if defined(OSCAR_SSL_SUPPORT) && BOS_SERVER_SUPPORTS_SSL
	return d_func()->account->config("connection").value("ssl", false);
#else
	return false;
#endif
}

AbstractConnection::State AbstractConnection::state() const
{
	return d_func()->state;
}

void AbstractConnection::registerInitializationSnacs(const QList<SNACInfo> &snacs, bool append)
{
	Q_D(AbstractConnection);
	if (append)
		d->initSnacs += snacs.toSet();
	else
		d->initSnacs = snacs.toSet();
}

void AbstractConnection::registerInitializationSnac(quint16 family, quint16 subtype)
{
	d_func()->initSnacs.insert(SNACInfo(family, subtype));
}

AbstractConnection::AbstractConnection(AbstractConnectionPrivate *d):
	d_ptr(d)
{
	d_func()->state = Unconnected;
}

const FLAP &AbstractConnection::flap()
{
	return d_func()->flap;
}

void AbstractConnection::send(SNAC &snac, bool priority)
{
	Q_D(AbstractConnection);
	OscarRate *rate = d->ratesHash.value(snac.family() << 16 | snac.subtype());
	if (!rate)
		// The first rate class is used by default.
		rate = d->rates.value(1);
	if (rate)
		rate->send(snac, priority);
	else
		sendSnac(snac);
}

void AbstractConnection::sendSnac(quint16 family, quint16 subtype, bool priority)
{
	SNAC snac(family, subtype);
	send(snac, priority);
}

void AbstractConnection::send(FLAP &flap)
{
	Q_D(AbstractConnection);
	flap.setSeqNum(d->seqNum());
	//debug(VeryVerbose) << "FLAP:" << flap.toByteArray().toHex().constData();
	d->socket->write(flap);
	//d->socket->flush();
}

bool AbstractConnection::testRate(quint16 family, quint16 subtype, bool priority)
{
	OscarRate *rate = d_func()->ratesHash.value(family << 16 | subtype);
	return rate ? rate->testRate(priority) : true;
}

quint32 AbstractConnection::sendSnac(SNAC &snac)
{
	Q_D(AbstractConnection);
	QString dbgStr;
	quint32 id = 0;
	// Not allow any snacs in unconnected state
	if (d->state == Unconnected) {
		dbgStr = "Trying to send SNAC(0x%1, 0x%2) to %3 which is in unconnected state";
	}
	// In connecting state, allow only snacs from whitelist
	else if (d->state == Connecting && !d->initSnacs.contains(SNACInfo(snac.family(), snac.subtype()))) {
		dbgStr = "Trying to send SNAC(0x%1, 0x%2) to %3 which is in connecting state";
	} else {
		dbgStr = "SNAC(0x%1, 0x%2) is sent to %3";
		// Send this snac
		FLAP flap(0x02);
		id = d->nextId();
		snac.setId(id);
		flap.append(snac.toByteArray());
		snac.lock();
		send(flap);
	}
    qWarning() << dbgStr
					  .arg(snac.family(), 4, 16, QChar('0'))
					  .arg(snac.subtype(), 4, 16, QChar('0'))
					  .arg(metaObject()->className());
	return id;
}

void AbstractConnection::setSeqNum(quint16 seqnum)
{
	d_func()->seqnum = seqnum;
}

void AbstractConnection::processNewConnection()
{
	qWarning() << QString("processNewConnection: %1 %2 %3")
					  .arg(flap().channel(), 2, 16, QChar('0'))
					  .arg(flap().seqNum())
					  .arg(flap().data().toHex().constData());
	setState(Connecting);
}

void AbstractConnection::processCloseConnection()
{
	Q_D(AbstractConnection);
	qWarning() << QString("processCloseConnection: %1 %2 %3")
					  .arg(d->flap.channel(), 2, 16, QChar('0'))
					  .arg(d->flap.seqNum())
					  .arg(d->flap.data().toHex().constData());
	FLAP flap(0x04);
	flap.append<quint32>(0x00000001);
	send(flap);
	socket()->disconnectFromHost();
}

void AbstractConnection::onDisconnect()
{
	setState(Unconnected);
	d_func()->aliveTimer.stop();
}

void AbstractConnection::onError(ConnectionError error)
{
	if (error != NoError)
		d_func()->socket->close();
}

void AbstractConnection::setError(ConnectionError e, const QString &errorStr)
{
	Q_D(AbstractConnection);
	d->error = e;
	d->errorStr = errorStr;
	if (d->error != NoError) {
		onError(e);
		emit error(e);
	}
}

void AbstractConnection::handleSNAC(AbstractConnection *conn, const SNAC &sn)
{
	Q_ASSERT(this == conn);
	Q_D(AbstractConnection);
	switch ((sn.family() << 16) | sn.subtype()) {
	// Server sends supported services list
	case ServiceFamily << 16 | ServiceServerReady: {
		QList<quint16> &services = d->services;
		while (sn.dataSize() != 0)
			services << sn.read<quint16>();
		SNAC snac(ServiceFamily, ServiceClientFamilies);
		// Sending the same as ICQ 6
		snac.append<quint32>(0x00220001);
		snac.append<quint32>(0x00010004);
		snac.append<quint32>(0x00130004);
		snac.append<quint32>(0x00020001);
		snac.append<quint32>(0x00030001);
		snac.append<quint32>(0x00150001);
		snac.append<quint32>(0x00040001);
		snac.append<quint32>(0x00060001);
		snac.append<quint32>(0x00090001);
		snac.append<quint32>(0x000a0001);
		snac.append<quint32>(0x000b0001);
		send(snac);
		break;
	}
		// This is the reply to CLI_REQINFO
	case ServiceFamily << 16 | ServiceServerNameInfo: {
		// Skip uin
		sn.read<QByteArray, quint8>();
		sn.skipData(4);
		// TLV(x01) User type?
		// TLV(x0C) Empty CLI2CLI Direct connection info
		// TLV(x0A) External IP
		// TLV(x0F) Number of seconds that user has been online
		// TLV(x03) The online since time.
		// TLV(x0A) External IP again
		// TLV(x22) Unknown
		// TLV(x1E) Unknown: empty.
		// TLV(x05) Member of ICQ since.
		// TLV(x14) Unknown
		TLVMap tlvs = sn.read<TLVMap>();
		quint32 ip = tlvs.value(0x0a).read<quint32>();
		d->ext_ip = QHostAddress(ip);

		//qDebug() << conn->externalIP();
		break;
	}
		// Server sends its services version numbers
	case ServiceFamily << 16 | ServiceServerFamilies2: {
		SNAC snac(ServiceFamily, ServiceClientReqRateInfo);
		send(snac);
		break;
	}
		// Server sends rate limits information
	case ServiceFamily << 16 | ServiceServerAsksServices: {
		foreach(const OscarRate *rate, d->rates)
			delete rate;
		d->rates.clear();
		d->ratesHash.clear();

		// Rate classes
		quint16 groupCount = sn.read<quint16>();
		for (int i = 0; i < groupCount; ++i) {
			OscarRate *rate = new OscarRate(sn, this);
			if (rate->isEmpty())
				continue;
			if (rate->groupId() == 1) // the first rate class will be used by default anyway
				d->rates.insert(rate->groupId(), rate);
		}
		// Rate groups
		while (sn.dataSize() >= 4) {
			quint16 groupId = sn.read<quint16>();
			quint16 count = sn.read<quint16>();
			QHash<quint16, OscarRate*>::iterator rateItr = d->rates.find(groupId);
			if (rateItr == d->rates.end()) {
				sn.skipData(count * 4);
				continue;
			}
			for (int j = 0; j < count; ++j) {
				quint32 snacType = sn.read<quint32>();
				d->ratesHash.insert(snacType, *rateItr);
			}
		}

		// Accepting rates
		SNAC snac(ServiceFamily, ServiceClientRateAck);
		for (int i = 1; i <= groupCount; i++)
			snac.append<quint16>(i);
		send(snac);
		break;
	}
	case ServiceFamily << 16 | ServiceServerRateChange: {
		sn.read<QByteArray, quint16>(); // Unknown
		quint16 code = sn.read<quint16>();
		if (code == 2)
			qDebug() << "Rate limits warning";
		if (code == 3)
			qDebug() << "Rate limits hit";
		if (code == 4)
			qDebug() << "Rate limits clear";
		quint32 groupId = sn.read<quint16>();
		if (d->rates.contains(groupId))
			d->rates.value(groupId)->update(sn);
		break;
	}
	case ServiceFamily << 16 | ServiceError: {
		ProtocolError error(sn);
		qDebug() << QString("Error (%1, %2): %3")
				   .arg(error.code(), 2, 16)
				   .arg(error.subcode(), 2, 16)
				   .arg(error.errorString());
		break;
	}
	}
}

void AbstractConnection::setState(AbstractConnection::State state)
{
	Q_D(AbstractConnection);
	d->state = state;
	if (state == Connected)
		d->aliveTimer.start();
}

quint16 AbstractConnection::generateFlapSequence()
{
	quint32 n = qrand(), s = 0;
	for (quint32 i = n; i >>= 3; s += i);
	return ((((0 - s) ^ (quint8) n) & 7) ^ n) + 2;
}

void AbstractConnection::processSnac()
{
	Q_D(AbstractConnection);
	SNAC snac = SNAC::fromByteArray(d->flap.data());
	qWarning() << QString("SNAC(0x%1, 0x%2) is received from %3")
					  .arg(snac.family(), 4, 16, QChar('0'))
					  .arg(snac.subtype(), 4, 16, QChar('0'))
					  .arg(metaObject()->className());
	bool found = false;
	foreach(SNACHandler *handler, d->handlers.values((snac.family() << 16)| snac.subtype())) {
		found = true;
		snac.resetState();
		handler->handleSNAC(this, snac);
	}
	if (!found) {
		qWarning() << QString("No handlers for SNAC(0x%1, 0x%2) in %3")
					 .arg(snac.family(), 4, 16, QChar('0'))
					 .arg(snac.subtype(), 4, 16, QChar('0'))
					 .arg(metaObject()->className());
	}
}

void AbstractConnection::readData()
{
	Q_D(AbstractConnection);
	if (d->socket->bytesAvailable() <= 0) {
		qDebug() << "readyRead emmited but the socket is empty";
		return;
	}
	if (d->flap.readData(d->socket)) {
		if (d->flap.isFinished()) {
			switch (d->flap.channel()) {
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
				qDebug() << "Unknown shac channel" << hex << d->flap.channel();
			case 0x03:
				break;
			case 0x05:
				qDebug() << "Connection alive!";
				break;
			}
			d->flap.clear();
		}
		// Just give a chance to other parts of qutIM to do something if needed
		if (d->socket->bytesAvailable())
			QTimer::singleShot(0, this, SLOT(readData()));
	} else {
		qCritical() << "Strange situation at" << Q_FUNC_INFO << ":" << __LINE__;
		d->socket->close();
	}
}

void AbstractConnection::stateChanged(QAbstractSocket::SocketState state)
{
	if (state == QAbstractSocket::ConnectedState)
		SystemIntegration::keepAlive(d_func()->socket);

	qWarning() << "New connection state" << state << this->metaObject()->className();
	if (state == QAbstractSocket::UnconnectedState) {
		onDisconnect();
		emit disconnected();
	}
}

void AbstractConnection::error(QAbstractSocket::SocketError error)
{
	setError(SocketError);
	qDebug() << "Connection error:" << error << errorString();
}

void AbstractConnection::sendAlivePacket()
{
	FLAP flap(0x05);
	flap.append<quint16>(0);
	send(flap);
	qDebug() << "Alive packet has been sent";
}

} } // namespace qutim_sdk_0_3::oscar


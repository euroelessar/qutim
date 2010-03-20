/****************************************************************************
 *  connection.cpp
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

#include "connection_p.h"
#include <QHostInfo>
#include <QBuffer>
#include <QCoreApplication>
#include <QNetworkProxy>

namespace qutim_sdk_0_3 {

namespace oscar {

ProtocolError::ProtocolError(const SNAC &snac)
{
	m_code = snac.read<qint16>();
	m_subcode = 0;
	TLVMap tlvs = snac.read<TLVMap>();
	if (tlvs.contains(0x08)) {
		DataUnit data(tlvs.value(0x08));
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
	update(m_groupId, sn);
	connect(&m_timer, SIGNAL(timeout()), SLOT(sendNextPackets()));
	m_timer.setSingleShot(true);
}

void OscarRate::update(quint32 groupId, const SNAC &sn)
{
	m_windowSize = sn.read<quint32>();
	m_clearLevel = sn.read<quint32>();
	m_alertLevel = sn.read<quint32>();
	m_limitLevel = sn.read<quint32>();
	m_disconnectLevel = sn.read<quint32>();
	m_currentLevel = sn.read<quint32>();
	m_maxLevel = sn.read<quint32>();
	m_lastTimeDiff = sn.read<quint32>();
	m_currentState = sn.read<quint8>();

	if (m_windowSize > 1) {
		m_time = QDateTime::currentDateTime();
		m_levelMultiplier = (m_windowSize - 1) / (double) m_windowSize;
		m_timeMultiplier = 1 / (double) m_windowSize;
	}
}

void OscarRate::send(const SNAC &snac, bool priority)
{
	if (priority)
		m_priorQueue.push_back(snac);
	else
		m_queue.push_back(snac);
	if (m_priorQueue.size() + m_queue.size() == 1)
		sendNextPackets();
}

void OscarRate::sendNextPackets()
{
	Q_ASSERT(!m_priorQueue.isEmpty() || !m_queue.isEmpty());
	QDateTime dateTime = QDateTime::currentDateTime();
	quint32 timeDiff;
	if (dateTime.date() == m_time.date())
		timeDiff = m_time.time().msecsTo(dateTime.time());
	else if (m_time.daysTo(dateTime) == 1)
		timeDiff = 86400000 - m_time.time().msec() + dateTime.time().msec();
	else // That should never happen
		timeDiff = 86400000;

	double levelInc = m_timeMultiplier * (double) timeDiff;
	quint32 newLevel = m_currentLevel * m_levelMultiplier + levelInc;
	while (newLevel >= m_clearLevel) {
		SNAC snac;
		if (!m_priorQueue.isEmpty())
			snac = m_priorQueue.takeFirst();
		else if (!m_queue.isEmpty())
			snac = m_queue.takeFirst();
		if (snac.isEmpty())
			break;
		m_currentLevel = newLevel;
		m_conn->sendSnac(snac);
		newLevel = newLevel * m_levelMultiplier;
	}
	m_currentLevel = qMin(m_currentLevel, m_maxLevel);
	m_time = dateTime;
	m_lastTimeDiff = timeDiff;
	if (!m_priorQueue.isEmpty() || !m_queue.isEmpty()) {
		Q_ASSERT(m_clearLevel > newLevel);
		quint32 timeout = (m_clearLevel - newLevel) / m_levelMultiplier / m_timeMultiplier;
		m_timer.start(timeout);
	}
}

AbstractConnection::AbstractConnection(IcqAccount *account, QObject *parent) :
	QObject(parent), d_ptr(new AbstractConnectionPrivate)
{
	Q_D(AbstractConnection);
	d->socket = new Socket(this);
#if OSCAR_SSL_SUPPORT
	d->socket->setPeerVerifyMode(QSslSocket::VerifyNone); // TODO:
#endif
	d->account = account;
	connect(d->account, SIGNAL(settingsUpdated()), SLOT(loadProxy()));
	connect(d->socket, SIGNAL(readyRead()), SLOT(readData()));
	connect(d->socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), SLOT(stateChanged(QAbstractSocket::SocketState)));
	connect(d->socket, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(error(QAbstractSocket::SocketError)));
	{
		ClientInfo info =
		{ "ICQ Client", 266, 6, 5, 10, 104, 0x00007537, "ru", "ru" };
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
	if (force) {
		d->socket->disconnectFromHost();
	} else {
		FLAP flap(0x04);
		flap.append<quint32>(0x00000001);
		send(flap);
	}
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
};

bool AbstractConnection::isConnected()
{
	return d_func()->socket->state() != QTcpSocket::UnconnectedState;
}

AbstractConnection::ConnectionError AbstractConnection::error()
{
	return d_func()->error;
};

void AbstractConnection::loadProxy()
{
	Q_D(AbstractConnection);
	QNetworkProxy proxy(QNetworkProxy::NoProxy);
	ConfigGroup cfg = d->account->config("connection");
	if (cfg.value("useproxy", false)) {
		cfg = d->account->config("proxy");
		QNetworkProxy::ProxyType type;
		QString typeStr = cfg.value("type", QString());
		if (typeStr.isNull()) {
			// attempt to load settings from qutim 0.2
			type = static_cast<QNetworkProxy::ProxyType>(cfg.value("proxyType", -1));
			if (type == -1) {
				type = QNetworkProxy::NoProxy;
			} else {
				// migration to the new format
				if (type == QNetworkProxy::Socks5Proxy)
					typeStr = "socks5";
				else if (type == QNetworkProxy::HttpProxy)
					typeStr = "http";
				else
					type = QNetworkProxy::NoProxy;
				if (!typeStr.isNull())
					cfg.setValue("type", typeStr);
				cfg.removeGroup("proxyType");
				cfg.sync();
			}
		} else if (typeStr == "socks5") {
			type = QNetworkProxy::Socks5Proxy;
		} else if (typeStr == "http") {
			type = QNetworkProxy::HttpProxy;
		}
		proxy.setHostName(cfg.value("host", QString()));
		proxy.setPort(cfg.value("port", 1));
		proxy.setType(type);
		if (cfg.value("auth", false)) {
			proxy.setUser(cfg.value("user", QString()));
			proxy.setPassword(cfg.value("pass", QString()));
		}
	}
	d->socket->setProxy(proxy);
}

QString AbstractConnection::errorString()
{
	Q_D(AbstractConnection);
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
		return QCoreApplication::translate("ConnectionError", "The users num connected from this IP has reached the maximum (reservation)");
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
		return QCoreApplication::translate("ConnectionError", "Another client is loggin with this uin");
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

bool AbstractConnection:: isSslEnabled()
{
#ifdef OSCAR_SSL_SUPPORT
	return d_func()->account->config("connection").value("ssl", false);
#else
	return false;
#endif
}

AbstractConnection::AbstractConnection(AbstractConnectionPrivate *d):
	d_ptr(d)
{
}

const FLAP &AbstractConnection::flap()
{
	return d_func()->flap;
}

void AbstractConnection::send(SNAC &snac, bool priority)
{
	Q_D(AbstractConnection);
	OscarRate *rate = d->ratesHash.value(snac.family() << 16 | snac.subtype());
	if (rate)
		rate->send(snac, priority);
	else
		sendSnac(snac);
}

void AbstractConnection::send(FLAP &flap)
{
	Q_D(AbstractConnection);
	flap.setSeqNum(d->seqNum());
	//debug(VeryVerbose) << "FLAP:" << flap.toByteArray().toHex().constData();
	d->socket->write(flap);
	//d->socket->flush();
}

quint32 AbstractConnection::sendSnac(SNAC &snac)
{
	Q_D(AbstractConnection);
	debug(Verbose) << QString("SNAC(0x%1, 0x%2) is sent to %3")
			.arg(snac.family(), 4, 16, QChar('0'))
			.arg(snac.subtype(), 4, 16, QChar('0'))
			.arg(metaObject()->className());
	FLAP flap(0x02);
	quint32 id = d->nextId();
	snac.setId(id);
	flap.append(snac.toByteArray());
	send(flap);
	return id;
}

void AbstractConnection::setSeqNum(quint16 seqnum)
{
	// Have a look at seqNum method to understand reasons
	d_func()->seqnum = (seqnum > 0) ? (seqnum - 1) : 0x7fff;
}

void AbstractConnection::processNewConnection()
{
	debug(Verbose) << QString("processNewConnection: %1 %2 %3")
			.arg(flap().channel(), 2, 16, QChar('0'))
			.arg(flap().seqNum())
			.arg(flap().data().toHex().constData());
}

void AbstractConnection::processCloseConnection()
{
	Q_D(AbstractConnection);
	debug(Verbose) << QString("processCloseConnection: %1 %2 %3")
			.arg(d->flap.channel(), 2, 16, QChar('0'))
			.arg(d->flap.seqNum())
			.arg(d->flap.data().toHex().constData());
	FLAP flap(0x04);
	flap.append<quint32>(0x00000001);
	send(flap);
	socket()->disconnectFromHost();
}

void AbstractConnection::setError(ConnectionError e)
{
	Q_D(AbstractConnection);
	d->error = e;
	if (d->error != NoError)
		emit error(d->error);
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
		//debug() << conn->externalIP();
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
			if (!rate->isEmpty())
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
				rateItr.value()->addSnacType(snacType);
				d->ratesHash.insert(snacType, *rateItr);
			}
		}

		// Accepting rates
		SNAC snac(ServiceFamily, ServiceClientRateAck);
		for (int i = 1; i <= groupCount; i++)
			snac.append<quint16>(i);
		send(snac);

		// This command requests from the server certain information about the client that is stored on the server
		// In other words: CLI_REQINFO
		snac.reset(ServiceFamily, ServiceClientReqinfo);
		break;
	}
	case ServiceFamily << 16 | ServiceServerRateChange: {
		sn.read<QByteArray, quint16>(); // Unknown
		quint16 code = sn.read<quint16>();
		if (code == 2)
			debug() << "Rate limits warning";
		if (code == 3)
			debug() << "Rate limits hit";
		if (code == 4)
			debug() << "Rate limits clear";
		quint32 groupId = sn.read<quint16>();
		if (d->rates.contains(groupId))
			d->rates.value(groupId)->update(groupId, sn);
		break;
	}
	case ServiceFamily << 16 | ServiceError: {
		ProtocolError error(sn);
		debug() << QString("Error (%1, %2): %3")
				.arg(error.code(), 2, 16)
				.arg(error.subcode(), 2, 16)
				.arg(error.errorString());
		break;
	}
	}
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
	debug(Verbose) << QString("SNAC(0x%1, 0x%2) is received from %3")
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
		warning() << QString("No handlers for SNAC(0x%1, 0x%2) in %3")
				.arg(snac.family(), 4, 16, QChar('0'))
				.arg(snac.subtype(), 4, 16, QChar('0'))
				.arg(metaObject()->className());
	}
}

void AbstractConnection::readData()
{
	Q_D(AbstractConnection);
	if (d->socket->bytesAvailable() <= 0) {
		debug() << "readyRead emmited but the socket is empty";
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
				debug() << "Unknown shac channel" << hex << d->flap.channel();
			case 0x03:
			case 0x05:
				break;
			}
			d->flap.clear();
		}
		// Just give a chance to other parts of qutIM to do something if needed
		if (d->socket->bytesAvailable())
			QTimer::singleShot(0, this, SLOT(readData()));
	} else {
		critical() << "Strange situation at" << Q_FUNC_INFO << ":" << __LINE__;
		d->socket->close();
	}
}

void AbstractConnection::stateChanged(QAbstractSocket::SocketState state)
{
	debug(Verbose) << "New connection state" << state << this->metaObject()->className();
}

void AbstractConnection::error(QAbstractSocket::SocketError error)
{
	debug() << "Connection error:" << error << errorString();
}

} } // namespace qutim_sdk_0_3::oscar

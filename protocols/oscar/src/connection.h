/****************************************************************************
 *  connection.h
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

#ifndef CONNECTION_H
#define CONNECTION_H

#include <QSslSocket>
#ifdef QT_NO_OPENSSL
# include <QTcpSocket>
#endif
//#ifdef OSCAR_SSL_SUPPORT
//#include <QSslSocket>
//#else
//#include <QTcpSocket>
//#endif
#include <QMap>
#include <QHostAddress>
#include <qutim/libqutim_global.h>
#include "snachandler.h"
#include "flap.h"
#include <qutim/dataforms.h>

#ifndef OSCAR_SSL_SUPPORT
# define OSCAR_SSL_SUPPORT
#endif

namespace qutim_sdk_0_3 {

namespace oscar {

class OscarRate;
class IcqAccount;
class AbstractConnectionPrivate;

struct LIBOSCAR_EXPORT ProtocolError
{
public:
	ProtocolError(const SNAC &snac);
	qint16 code() { return m_code; }
	qint16 subcode() { return m_subcode; }
	QString errorString();
	TLVMap tlvs() const { return m_tlvs; }
protected:
	qint16 m_code;
	qint16 m_subcode;
	TLVMap m_tlvs;
};

struct LIBOSCAR_EXPORT ClientInfo
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

struct LIBOSCAR_EXPORT DirectConnectionInfo
{
	QHostAddress internal_ip;
	QHostAddress external_ip;
	quint32 port;
	quint8 dc_type;
	quint16 protocol_version;
	quint32 auth_cookie;
	quint32 web_front_port;
	quint32 client_features;
	quint32 info_utime; // last info update time (unix time_t)
	quint32 extinfo_utime; // last ext info update time (i.e. icqphone status)
	quint32 extstatus_utime; // last ext status update time (i.e. phonebook)
};

#ifdef OSCAR_SSL_SUPPORT
typedef QSslSocket Socket;
#else
typedef QTcpSocket Socket;
#endif


class LIBOSCAR_EXPORT AbstractConnection : public QObject, public SNACHandler
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::oscar::SNACHandler)
	Q_DECLARE_PRIVATE(AbstractConnection)
public:
	enum ConnectionError
	{
		NoError = 0x00,
		InvalidNickOrPassword = 0x01,
		ServiceUnaivalable = 0x02,
		IncorrectNickOrPassword = 0x04,
		MismatchNickOrPassword = 0x05,
		InternalClientError = 0x06,
		InvalidAccount = 0x07,
		DeletedAccount = 0x08,
		ExpiredAccount = 0x09,
		NoAccessToDatabase = 0x0a,
		NoAccessToResolver = 0x0b,
		InvalidDatabaseFields = 0x0c,
		BadDatabaseStatus = 0x0D,
		BadResolverStatus = 0x0E,
		InternalError = 0x0F,
		ServiceOffline = 0x10,
		SuspendedAccount = 0x11,
		DBSendError = 0x12,
		DBLinkError = 0x13,
		ReservationMapError = 0x14,
		ReservationLinkError = 0x15,
		ConnectionLimitExceeded = 0x16,
		ConnectionLimitExceededReservation = 0x17,
		RateLimitExceededReservation = 0x18,
		UserHeavilyWarned = 0x19,
		ReservationTimeout = 0x1a,
		ClientUpgradeRequired = 0x1b,
		ClientUpgradeRecommended = 0x1c,
		RateLimitExceeded = 0x1d,
		IcqNetworkError = 0x1e,
		InvalidSecirID = 0x20,
		AgeLimit = 0x22,
		AnotherClientLogined = 0x80,
		SocketError = 0x81,
		HostNotFound = 0x82
	};
	enum State
	{
		Unconnected,
		Connecting,
		Connected
	};

public:
	explicit AbstractConnection(IcqAccount *account, QObject *parent = 0);
	virtual ~AbstractConnection();
	void registerHandler(SNACHandler *handler);
	void send(SNAC &snac, bool priority = true);
	bool testRate(quint16 family, quint16 subtype, bool priority = true);
	void disconnectFromHost(bool force = false);
	const QHostAddress &externalIP() const;
	const QList<quint16> &servicesList();
	Socket *socket();
	const Socket *socket() const;
	ConnectionError error();
	QString errorString();
	IcqAccount *account();
	const IcqAccount *account() const;
	const ClientInfo &clientInfo();
	bool isSslEnabled();
	State state() const;
	void registerInitializationSnacs(const QList<SNACInfo> &snacs, bool append = true);
	void registerInitializationSnac(quint16 family, quint16 subtype);
signals:
	void error(qutim_sdk_0_3::oscar::AbstractConnection::ConnectionError error);
	void disconnected();
protected:
	AbstractConnection(AbstractConnectionPrivate *d);
	const FLAP &flap();
	void send(FLAP &flap);
	quint32 sendSnac(SNAC &snac);
	void setSeqNum(quint16 seqnum);
	virtual void processNewConnection();
	virtual void processCloseConnection();
	virtual void onDisconnect();
	virtual void onError(ConnectionError error);
	void setError(ConnectionError error, const QString &errorStr = QString());
	virtual void handleSNAC(AbstractConnection *conn, const SNAC &snac);
	void setState(AbstractConnection::State state);
	static quint16 generateFlapSequence();
protected slots:
	void setProxy(const QNetworkProxy &proxy);
private slots:
	void processSnac();
	void readData();
	void stateChanged(QAbstractSocket::SocketState);
	void error(QAbstractSocket::SocketError);
	void sendAlivePacket();
private:
	friend class OscarRate;
	QScopedPointer<AbstractConnectionPrivate> d_ptr;
};

} } // namespace qutim_sdk_0_3::oscar

#endif // CONNECTION_H

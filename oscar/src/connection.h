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

#include <QTcpSocket>
#include <QMap>
#include <QHostAddress>
#include <qutim/libqutim_global.h>
#include "snachandler.h"
#include "flap.h"

namespace qutim_sdk_0_3 {

namespace oscar {

extern quint16 generate_flap_sequence();

class OscarRate;

struct ProtocolError
{
public:
	ProtocolError(const SNAC &snac);
	qint16 code;
	qint16 subcode;
	QString str;
private:
	QString getErrorStr();
};

class ProtocolNegotiation : public QObject, public SNACHandler
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::oscar::SNACHandler)
public:
	ProtocolNegotiation(QObject *parent = 0);
	virtual void handleSNAC(AbstractConnection *conn, const SNAC &snac);
private:
	void setMsgChannelParams(AbstractConnection *conn, quint16 chans, quint32 flags);
	quint32 m_login_reqinfo;
};

class AbstractConnection : public QObject
{
	Q_OBJECT
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
		AnotherClientLogined = 0x80
	};
public:
	AbstractConnection(QObject *parent);
	~AbstractConnection();
	void registerHandler(SNACHandler *handler);
	void send(SNAC &snac, bool priority = true);
	void disconnectFromHost(bool force);
	void setExternalIP(const QHostAddress &ip) { m_ext_ip = ip; }
	const QHostAddress &externalIP() const { return m_ext_ip; }
	void setServicesList(const QList<quint16> &services) { m_services = services; };
	const QList<quint16> &servicesList() { return m_services; };
	QTcpSocket *socket() { return m_socket; };
	bool isConnected() { return m_socket->state() != QTcpSocket::UnconnectedState; }
	ConnectionError error() { return m_error; };
	QString errorString();
signals:
	void error(ConnectionError error);
protected:
	const FLAP &flap() { return m_flap; }
	void send(FLAP &flap);
	quint32 sendSnac(SNAC &snac);
	void setSeqNum(quint16 seqnum);
	virtual void processNewConnection();
	virtual void processCloseConnection();
	void setError(ConnectionError error);
private slots:
	void processSnac();
	void readData();
	void stateChanged(QAbstractSocket::SocketState);
	void error(QAbstractSocket::SocketError);
private:
	// max value is 0x7fff, min is 0
	inline quint16 seqNum() { m_seqnum++; return (m_seqnum &= 0x7fff); }
	inline quint32 nextId() { return m_id++; }
private:
	friend class ProtocolNegotiation;
	friend class OscarRate;
	QTcpSocket *m_socket;
	FLAP m_flap;
	QMultiMap<quint32, SNACHandler*> m_handlers;
	quint16 m_seqnum;
	quint32 m_id;
	QHostAddress m_ext_ip;
	QList<quint16> m_services;
	QHash<quint16, OscarRate*> m_rates;
	QHash<quint32, OscarRate*> m_ratesHash;
	ConnectionError m_error;
};

} } // namespace qutim_sdk_0_3::oscar

#endif // CONNECTION_H

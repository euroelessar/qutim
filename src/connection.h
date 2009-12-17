/****************************************************************************
 *  connection.h
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

#ifndef CONNECTION_H
#define CONNECTION_H

#include <QTcpSocket>
#include <QMap>
#include <QHostAddress>
#include <qutim/libqutim_global.h>
#include "snachandler.h"
#include "flap.h"

namespace Icq {

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

class ProtocolNegotiation: public SNACHandler
{
	Q_OBJECT
public:
	ProtocolNegotiation(QObject *parent = 0);
	virtual void handleSNAC(AbstractConnection *conn, const SNAC &snac);
private:
	void setMsgChannelParams(AbstractConnection *conn, quint16 chans, quint32 flags);
	quint32 m_login_reqinfo;
};

class AbstractConnection: public QObject
{
	Q_OBJECT
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
protected:
	const FLAP &flap() { return m_flap; }
	void send(FLAP &flap);
	quint32 sendSnac(SNAC &snac);
	void setSeqNum(quint16 seqnum);
	virtual void processNewConnection();
	virtual void processCloseConnection();
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
};

} // namespace Icq

#endif // CONNECTION_H

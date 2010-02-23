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

#ifndef CONNECTION_P_H
#define CONNECTION_P_H

#include "oscarconnection.h"
#include "snac.h"
#include <QTimer>
#include <QDateTime>

namespace qutim_sdk_0_3 {

namespace oscar {

struct OscarRate: public QObject
{
	Q_OBJECT
public:
	OscarRate(const SNAC &sn, AbstractConnection *conn);
	void update(quint32 groupId, const SNAC &sn);
	const QList<quint32> &snacTypes()
	{
		return m_snacTypes;
	}
	void addSnacType(quint32 snacType)
	{
		m_snacTypes << snacType;
	}
	quint16 groupId()
	{
		return m_groupId;
	}
	void send(const SNAC &snac, bool priority);
	bool isEmpty()
	{
		return m_windowSize <= 1;
	}
private slots:
	void sendNextPackets();
private:
	quint16 m_groupId;
	quint32 m_windowSize;
	quint32 m_clearLevel;
	quint32 m_alertLevel;
	quint32 m_limitLevel;
	quint32 m_disconnectLevel;
	quint32 m_currentLevel;
	quint32 m_maxLevel;
	quint32 m_lastTimeDiff;
	quint8 m_currentState;
	QDateTime m_time;
	QList<quint32> m_snacTypes;
	QList<SNAC> m_priorQueue;
	QList<SNAC> m_queue;
	double m_levelMultiplier;
	double m_timeMultiplier;
	QTimer m_timer;
	AbstractConnection *m_conn;
};

class AbstractConnectionPrivate
{
public:
	// max value is 0x7fff, min is 0
	inline quint16 seqNum() { seqnum++; return (seqnum &= 0x7fff); }
	inline quint32 nextId() { return id++; }
	QTcpSocket *socket;
	FLAP flap;
	QMultiMap<quint32, SNACHandler*> handlers;
	quint16 seqnum;
	quint32 id;
	QHostAddress ext_ip;
	QList<quint16> services;
	QHash<quint16, OscarRate*> rates;
	QHash<quint32, OscarRate*> ratesHash;
	AbstractConnection::ConnectionError error;
};

} } // namespace qutim_sdk_0_3::oscar

#endif //CONNECTION_P_H

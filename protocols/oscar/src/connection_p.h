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

#ifndef CONNECTION_P_H
#define CONNECTION_P_H

#include "connection.h"
#include "snac.h"
#include "icqaccount.h"
#include <QTimer>
#include <QDateTime>
#include <QQueue>

namespace qutim_sdk_0_3 {

namespace oscar {

#define MINIMIZE_RATE_MEMORY_USAGE 1

class OscarRate: public QObject
{
	Q_OBJECT
public:
	OscarRate(const SNAC &sn, AbstractConnection *conn);
	virtual ~OscarRate() {}
	void update(const SNAC &sn);
	quint16 groupId() { return m_groupId; }
	void send(const SNAC &snac, bool priority);
	bool isEmpty() { return m_windowSize <= 1; }
	bool testRate(bool priority);
	bool startTimeout();
protected:
	void timerEvent(QTimerEvent *event);
private:
	void sendNextPackets();
	quint32 getTimeDiff(const QDateTime &dateTime);
private:
	quint16 m_groupId;
	quint32 m_windowSize;
	quint32 m_clearLevel;
	quint32 m_currentLevel;
	quint32 m_maxLevel;
	quint32 m_lastTimeDiff;
#if !MINIMIZE_RATE_MEMORY_USAGE
	quint32 m_alertLevel;
	quint32 m_limitLevel;
	quint32 m_disconnectLevel;
	quint8 m_currentState;
#endif
	QDateTime m_time;
	QQueue<SNAC> m_lowPriorityQueue;
	QQueue<SNAC> m_highPriorityQueue;
	QBasicTimer m_timer;
	quint32 m_defaultPriority;
	AbstractConnection *m_conn;
};

class AbstractConnectionPrivate
{
public:
	inline quint16 seqNum() { return seqnum++; }
	inline quint32 nextId() { return id++; }
	Socket *socket;
	FLAP flap;
	QMultiMap<quint32, SNACHandler*> handlers;
	quint16 seqnum;
	quint32 id;
	ClientInfo clientInfo;
	QHostAddress ext_ip;
	QList<quint16> services;
	QHash<quint16, OscarRate*> rates;
	QHash<quint32, OscarRate*> ratesHash;
	AbstractConnection::ConnectionError error;
	QString errorStr;
	IcqAccount *account;
	AbstractConnection::State state;
	QSet<SNACInfo> initSnacs; // Snacs that are allowed when initializing connection
	QTimer aliveTimer;
};

} } // namespace qutim_sdk_0_3::oscar

#endif //CONNECTION_P_H


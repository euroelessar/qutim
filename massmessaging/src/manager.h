/****************************************************************************
 *  manager.h
 *
 *  Copyright (c) 2011 by Sidorov Aleksey <sauron@citadelspb.com>
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

#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include <QQueue>
#include <QBasicTimer>
#include "messaging.h"

class QStandardItem;
class QAbstractItemModel;
class QStandardItemModel;
namespace qutim_sdk_0_3 {
class Contact;
}
namespace MassMessaging
{

class Manager : public QObject
{
	Q_OBJECT
public:
	Manager(QObject* parent = 0);
	virtual ~Manager();
	QAbstractItemModel *model() const;
public slots:
	void reload();
	void start(const QString &message, int interval = 15000);
	void stop();
	bool currentState();
signals:
	void update (const uint &completed, const uint &total, const QString &text);
	void finished(bool ok);
private:
	virtual void timerEvent(QTimerEvent* ev);
	QString parseText(const QString &msg,Contact *c);
	QList<QStandardItem *> m_contacts;
	QQueue<QStandardItem *> m_recievers;
	QStandardItemModel *m_model;
	QBasicTimer m_timer; //just simple
	int m_total_item_count;
	QString m_message;
};
}

#endif // MANAGER_H

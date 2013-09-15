/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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
    QQueue<QStandardItem *> m_receivers;
    QStandardItemModel *m_model;
    QBasicTimer m_timer; //just simple
    int m_total_item_count;
    QString m_message;
};

#endif // MANAGER_H


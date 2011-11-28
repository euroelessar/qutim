/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#ifndef GROUPCHATMANAGER_P_H
#define GROUPCHATMANAGER_P_H

#include "groupchatmanager.h"
#include <QHash>

namespace qutim_sdk_0_3
{

class GroupChatManagerPrivate
{
public:
    Account *account;
};

class GroupChatManagersList : public QObject
{
    Q_OBJECT
public:
    void addManager(GroupChatManager *manager);
    void removeManager(GroupChatManager *manager);
    const QHash<Account*, GroupChatManager*> &managers() { return m_managers; }
    static GroupChatManagersList *instance();
private slots:
    void accountDestroyed(QObject *object);
private:
    QHash<Account*, GroupChatManager*> m_managers;
};

}

#endif // GROUPCHATMANAGER_P_H


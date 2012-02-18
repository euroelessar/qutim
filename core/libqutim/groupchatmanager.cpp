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
#include "groupchatmanager_p.h"
#include "account.h"

namespace qutim_sdk_0_3
{

void GroupChatManagersList::addManager(GroupChatManager *manager)
{
    Account *acc = manager->account();
    Q_ASSERT(!m_managers.contains(acc));
    m_managers.insert(acc, manager);
    connect(acc, SIGNAL(destroyed(QObject*)),
            this, SLOT(accountDestroyed(QObject*)));
}

void GroupChatManagersList::removeManager(GroupChatManager *manager)
{
    Account *acc = manager->account();
    int removed = m_managers.remove(acc);
    if (removed > 0) {
        disconnect(acc, SIGNAL(destroyed(QObject*)),
                   this, SLOT(accountDestroyed(QObject*)));
    }
}

GroupChatManagersList *GroupChatManagersList::instance()
{
    static GroupChatManagersList list;
    return &list;
}

void GroupChatManagersList::accountDestroyed(QObject *object)
{
    Account *acc = reinterpret_cast<Account*>(object);
    m_managers.remove(acc);
}

GroupChatManager::GroupChatManager(Account *account) :
    d(new GroupChatManagerPrivate)
{
    d->account = account;
}

GroupChatManager::~GroupChatManager()
{
}

Account *GroupChatManager::account() const
{
    return d->account;
}

QList<GroupChatManager*> GroupChatManager::allManagers()
{
    return GroupChatManagersList::instance()->managers().values();
}

}


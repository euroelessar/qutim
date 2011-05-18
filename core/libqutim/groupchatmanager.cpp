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

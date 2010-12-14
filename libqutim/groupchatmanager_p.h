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

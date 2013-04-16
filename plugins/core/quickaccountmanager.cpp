/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2013 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "quickaccountmanager.h"

typedef QList<Account*> (AccountManager::*GetListMethod) () const;

struct FunctionScope {
    GetListMethod method;
};
static FunctionScope functionScopes[] = {
    { &AccountManager::allAccounts },
    { &AccountManager::enabledAccounts }
};

// Keep in sync with above one
enum {
    AllAccountsIndex,
    EnabledAccountsIndex
};

static QList<Account*> accountListGet(QQmlListProperty<Account> *list)
{
    auto method = reinterpret_cast<FunctionScope *>(list->data)->method;
    return (*static_cast<AccountManager*>(list->object).*method)();
}

static int accountListCount(QQmlListProperty<Account> *list)
{
    return accountListGet(list).count();
}

static Account *accountListAt(QQmlListProperty<Account> *list, int index)
{
    return accountListGet(list).at(index);
}

static QQmlListProperty<Account> createPropertyList(const AccountManager *manager, FunctionScope *scope)
{
    return QQmlListProperty<Account>(const_cast<AccountManager*>(manager),
                                     scope,
                                     accountListCount,
                                     accountListAt);
}

QuickAccountManager::QuickAccountManager(QObject *parent) :
    AccountManager(parent)
{
    connect(this, &AccountManager::enabledAccountsChanged,
            this, &QuickAccountManager::onEnabledAccountsChanged);
    connect(this, &AccountManager::allAccountsChanged,
            this, &QuickAccountManager::onAllAccountsChanged);
}

QQmlListProperty<Account> QuickAccountManager::enabledAccounts() const
{
    return createPropertyList(this, &functionScopes[EnabledAccountsIndex]);
}

QQmlListProperty<Account> QuickAccountManager::allAccounts() const
{
    return createPropertyList(this, &functionScopes[AllAccountsIndex]);
}

void QuickAccountManager::onEnabledAccountsChanged(const QList<Account *> &)
{
    emit enabledAccountsChanged(enabledAccounts());
}

void QuickAccountManager::onAllAccountsChanged(const QList<Account *> &)
{
    emit allAccountsChanged(allAccounts());
}

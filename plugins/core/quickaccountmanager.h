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

#ifndef QUICKACCOUNTMANAGER_H
#define QUICKACCOUNTMANAGER_H

#include "accountmanager.h"
#include <QQmlListProperty>

class QuickAccountManager : public AccountManager
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<Account> enabledAccounts READ enabledAccounts NOTIFY enabledAccountsChanged)
    Q_PROPERTY(QQmlListProperty<Account> allAccounts READ allAccounts NOTIFY allAccountsChanged)
public:

    explicit QuickAccountManager(QObject *parent = 0);

    QQmlListProperty<Account> enabledAccounts() const;
    QQmlListProperty<Account> allAccounts() const;

Q_SIGNALS:
    void enabledAccountsChanged(const QQmlListProperty<Account> &enabledAccountsChanged);
    void allAccountsChanged(const QQmlListProperty<Account> &allAccountsChanged);

private:
    void onEnabledAccountsChanged(const QList<Account*> &accounts);
    void onAllAccountsChanged(const QList<Account*> &accounts);
};

#endif // QUICKACCOUNTMANAGER_H

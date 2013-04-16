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

#ifndef ACCOUNTMANAGER_H
#define ACCOUNTMANAGER_H

#include <QObject>
#include <qqml.h>
#include <QSharedPointer>
#include "account.h"

class AccountManagerPrivate;

class AccountManager : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(AccountManager)
    Q_PROPERTY(QList<Account*> enabledAccounts READ enabledAccounts NOTIFY enabledAccountsChanged)
    Q_PROPERTY(QList<Account*> allAccounts READ allAccounts NOTIFY allAccountsChanged)
    
public:
    AccountManager(QObject *parent = 0);
    ~AccountManager();

    QList<Account*> enabledAccounts() const;
    QList<Account*> allAccounts() const;

Q_SIGNALS:
    void enabledAccountsChanged(const QList<Account*> &accounts);
    void allAccountsChanged(const QList<Account*> &accounts);

private:
    friend class AccountManagerPrivate;
    QSharedPointer<AccountManagerPrivate> d;
};

QML_DECLARE_TYPE(AccountManager)

#endif // ACCOUNTMANAGER_H


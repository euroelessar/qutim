/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef UREEN_ACCOUNTMANAGER_H
#define UREEN_ACCOUNTMANAGER_H

#include <QObject>
#include <QStringList>
#include <QVariantMap>
#include "pendingreply.h"

namespace Ureen {
    class Account;
}

namespace Ureen {

typedef Ureen::Account Account;

class AccountManagerPrivate;

class AccountManager : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("Service", "AccountManager")
    Q_PROPERTY(QList<Ureen::Account*> validAccounts READ validAccounts NOTIFY validAccountsChanged)
    Q_PROPERTY(QList<Ureen::Account*> invalidAccounts READ invalidAccounts NOTIFY invalidAccountsChanged)
    Q_PROPERTY(QStringList supportedAccountProperties READ supportedAccountProperties NOTIFY supportedAccountPropertiesChanged)
public:
    static AccountManager *instance();

    QList<Ureen::Account*> validAccounts() const;
    QList<Ureen::Account*> invalidAccounts() const;
    QStringList supportedAccountProperties() const;

public slots:
    virtual PendingReply<Account*> createAccount(const QString &connectionManager, const QString &protocol, const QString &displayName,
                                                 const QVariantMap &parameters, const QVariantMap &properties) = 0;
    
signals:
    void validAccountsChanged(const QList<Ureen::Account*> &validAccounts);
    void validAccountCreated(Ureen::Account *account);
    void validAccountRemoved(Ureen::Account *account);
    void invalidAccountCreated(Ureen::Account *account);
    void invalidAccountRemoved(Ureen::Account *account);
    void invalidAccountsChanged(const QList<Ureen::Account*> &invalidAccounts);
    void supportedAccountPropertiesChanged(const QStringList &supportedAccountProperties);

protected:
    explicit AccountManager();
    ~AccountManager();

    void updateValidAccounts(const QList<Account*> &validAccounts);
    void updateInvalidAccounts(const QList<Account*> &invalidAccounts);
    void updateSupportedAccountProperties(const QStringList &supportedAccountProperties);

private:
    QScopedPointer<AccountManagerPrivate> d_ptr;
};

} // namespace Ureen

#endif // UREEN_ACCOUNTMANAGER_H

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

#include "accountmanager.h"
#include "servicemanager.h"

namespace Ureen {

class AccountManagerPrivate
{
public:
    QList<Ureen::Account*> validAccounts;
    QList<Ureen::Account*> invalidAccounts;
    QStringList supportedAccountProperties;
};

AccountManager::AccountManager() : d_ptr(new AccountManagerPrivate)
{
}

AccountManager::~AccountManager()
{
}

AccountManager *AccountManager::instance()
{
    static Ureen::ServicePointer<AccountManager> self;
    return self.data();
}

QList<Account *> AccountManager::validAccounts() const
{
    return d_ptr->validAccounts;
}

QList<Account *> AccountManager::invalidAccounts() const
{
    return d_ptr->invalidAccounts;
}

QStringList AccountManager::supportedAccountProperties() const
{
    return d_ptr->supportedAccountProperties;
}

void AccountManager::updateValidAccounts(const QList<Account *> &validAccounts)
{
    foreach (Account *account, validAccounts) {
        if (!d_ptr->validAccounts.contains(account))
            emit validAccountCreated(account);
    }
    foreach (Account *account, d_ptr->validAccounts) {
        if (!validAccounts.contains(account))
            emit validAccountRemoved(account);
    }
    d_ptr->validAccounts = validAccounts;
    emit validAccountsChanged(validAccounts);
}

void AccountManager::updateInvalidAccounts(const QList<Account *> &invalidAccounts)
{
    foreach (Account *account, invalidAccounts) {
        if (!d_ptr->invalidAccounts.contains(account))
            emit invalidAccountCreated(account);
    }
    foreach (Account *account, d_ptr->invalidAccounts) {
        if (!invalidAccounts.contains(account))
            emit invalidAccountRemoved(account);
    }
    d_ptr->invalidAccounts = invalidAccounts;
    emit invalidAccountsChanged(invalidAccounts);
}

void AccountManager::updateSupportedAccountProperties(const QStringList &supportedAccountProperties)
{
    d_ptr->supportedAccountProperties = supportedAccountProperties;
    emit supportedAccountPropertiesChanged(supportedAccountProperties);
}

} // namespace Ureen

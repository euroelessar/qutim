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

#include "accountmanager.h"
#include "protocol.h"
#include <QCoreApplication>
#include <QPointer>

namespace qutim_sdk_0_3 {

class AccountManagerPrivate
{
public:
	QList<Account*> accounts;
	QList<Account*> validAccounts;
	QList<Account*> invalidAccounts;

	void _q_onAccountCreated(qutim_sdk_0_3::Account *);
	void _q_onAccountRemoved(qutim_sdk_0_3::Account *);
};

static AccountManager *self = NULL;

void AccountManagerPrivate::_q_onAccountCreated(Account *account)
{
	accounts.append(account);
	validAccounts.append(account);
	emit self->accountsChanged(accounts);
	emit self->validAccountsChanged(validAccounts);
}

void AccountManagerPrivate::_q_onAccountRemoved(Account *account)
{
	validAccounts.removeOne(account);
	accounts.removeOne(account);
	emit self->invalidAccountsChanged(validAccounts);
	emit self->accountsChanged(accounts);
	emit self->accountRemoved(account);
}

AccountManager::AccountManager(QObject *parent) :
	QObject(parent), d_ptr(new AccountManagerPrivate)
{
	Q_D(AccountManager);
	Q_ASSERT(!self);
	self = this;

	foreach (Protocol *protocol, Protocol::all()) {
		d->validAccounts << protocol->accounts();
		connect(protocol, SIGNAL(accountCreated(qutim_sdk_0_3::Account*)),
				this, SLOT(_q_onAccountCreated(qutim_sdk_0_3::Account*)));
		connect(protocol, SIGNAL(accountRemoved(qutim_sdk_0_3::Account*)),
				this, SLOT(_q_onAccountRemoved(qutim_sdk_0_3::Account*)));
	}
}

AccountManager::~AccountManager()
{
	Q_ASSERT(self == this);
	self = NULL;
}

AccountManager *AccountManager::instance()
{
	return self;
}

QList<Account *> AccountManager::accounts() const
{
	return d_func()->accounts;
}

QList<Account *> AccountManager::validAccounts() const
{
	return d_func()->validAccounts;
}

QList<Account *> AccountManager::invalidAccounts() const
{
	return d_func()->invalidAccounts;
}

} // namespace qutim_sdk_0_3

#include "moc_accountmanager.cpp"

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

#include "accountmanager_p.h"

namespace qutim_sdk_0_3
{

AccountManager *AccountManagerPrivate::self = NULL;

void AccountManagerPrivate::addProtocol(Protocol *protocol)
{
	Q_Q(AccountManager);
	protocols.insert(protocol->id(), protocol);

	QObject::connect(protocol, &Protocol::accountCreated, q, [this, q] (Account *account) {
		Q_ASSERT(accounts.count(account) == 0);
		accounts.append(account);

		emit q->accountAdded(account);
	});
	QObject::connect(protocol, &Protocol::accountRemoved, q, [this, q] (Account *account) {
		Q_ASSERT(accounts.count(account) == 1);
		accounts.removeOne(account);

		emit q->accountRemoved(account);
	});
}

AccountManager::AccountManager() : d_ptr(new AccountManagerPrivate(this))
{
}

AccountManager::~AccountManager()
{
}

AccountManager *AccountManager::instance()
{
	return AccountManagerPrivate::self;
}

QList<Account *> AccountManager::accounts() const
{
	return d_func()->accounts;
}

}

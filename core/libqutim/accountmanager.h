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

#ifndef QUTIM_SDK_0_3_ACCOUNTMANAGER_H
#define QUTIM_SDK_0_3_ACCOUNTMANAGER_H

#include "account.h"

namespace qutim_sdk_0_3 {

class AccountManagerPrivate;
class ModuleManager;

class LIBQUTIM_EXPORT AccountManager : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(AccountManager)
	Q_PROPERTY(QList<qutim_sdk_0_3::Account*> validAccounts READ validAccounts NOTIFY validAccountsChanged)
	Q_PROPERTY(QList<qutim_sdk_0_3::Account*> invalidAccounts READ invalidAccounts NOTIFY invalidAccountsChanged)
private:
	explicit AccountManager(QObject *parent = 0);

public:
	~AccountManager();

	static AccountManager *instance();

	QList<qutim_sdk_0_3::Account*> accounts() const;
	QList<qutim_sdk_0_3::Account*> validAccounts() const;
	QList<qutim_sdk_0_3::Account*> invalidAccounts() const;
    
signals:
	void accountCreated(qutim_sdk_0_3::Account *account);
	void accountRemoved(qutim_sdk_0_3::Account *account);
	void accountsChanged(const QList<qutim_sdk_0_3::Account*> &accounts);
	void validAccountsChanged(const QList<qutim_sdk_0_3::Account*> &validAccounts);
	void invalidAccountsChanged(const QList<qutim_sdk_0_3::Account*> &invalidAccounts);

private:
	Q_PRIVATE_SLOT(d_func(), void _q_onAccountCreated(qutim_sdk_0_3::Account*))
	Q_PRIVATE_SLOT(d_func(), void _q_onAccountRemoved(qutim_sdk_0_3::Account*))

	friend class ModuleManager;
	QScopedPointer<AccountManagerPrivate> d_ptr;
};

} // namespace qutim_sdk_0_3

#endif // QUTIM_SDK_0_3_ACCOUNTMANAGER_H

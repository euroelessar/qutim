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

#ifndef ACCOUNT_P_H
#define ACCOUNT_P_H

#include "menucontroller_p.h"
#include "account.h"
#include "protocol.h"
#include "status.h"
#include "groupchatmanager_p.h"
#include "rosterstorage.h"

#include <unordered_map>
#include <memory>

namespace qutim_sdk_0_3
{
struct AccountInterface
{
	std::unique_ptr<QObject> object;
};

class AccountPrivate : public MenuControllerPrivate
{
	Q_DECLARE_PUBLIC(Account)
public:
	AccountPrivate(Account *a) : MenuControllerPrivate(a), state(Account::Disconnected) {}

	void updateStatus();
	void setStatus(const Status &newStatus);

	QPointer<Protocol> protocol;
	QString id;
	Account::State state;
	Status status;
	Status userStatus;

	struct hasher
	{
		size_t operator() (const QByteArray &array) const
		{
			return qHash(array);
		}
	};

	std::unordered_map<QByteArray, AccountInterface, hasher> interfaces;
};
}

#endif // ACCOUNT_P_H


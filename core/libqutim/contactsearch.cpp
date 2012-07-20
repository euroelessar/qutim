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
#include "contactsearch.h"
#include "abstractsearchrequest_p.h"
#include "dynamicpropertydata_p.h"
#include <QSet>
#include "protocol.h"
#include "account.h"
#include "contact.h"
#include "icon.h"

namespace qutim_sdk_0_3
{
class ContactSearchFactoryPrivate : public AbstractSearchFactoryPrivate
{
};

class GeneralContactSearchFactoryPrivate : public ContactSearchFactoryPrivate
{
public:
	struct Item
	{
		Item() :
			account(0)
		{
		}
		Item(Account *account_) :
			account(account_),
			id(account->id()),
			status(account->status())
		{
		}
		bool isActive() const{ return status != Status::Offline; }
		Account *account;
		QString id;
		Status status;
	};
	Item addAccount(Account *account);
	GeneralContactSearchFactory *q;
	QHash<QString, Item> accounts;
	Protocol *protocol;
};
typedef GeneralContactSearchFactoryPrivate::Item AccountItem;
typedef QHash<QString, AccountItem> AccountsHash;

ContactSearchRequest::ContactSearchRequest()
{
}

ContactSearchRequest::~ContactSearchRequest()
{
}


int ContactSearchRequest::actionCount() const
{
	return 1;
}

QVariant ContactSearchRequest::actionData(int index, int role)
{
	if (index == 0) {
		if (role == Qt::DisplayRole) {
			return tr("Add contact");
		} else if (role == Qt::DecorationRole) {
			return Icon("edit-add-contact");
		}
	}
	return QVariant();
}

void ContactSearchRequest::actionActivated(int actionIndex, int row)
{
	if (actionIndex == 0) {
		Contact *c = contact(row);
		if (c)
			c->addToList();
	}
}

ContactSearchFactory::ContactSearchFactory() :
	AbstractSearchFactory()
{
}

ContactSearchFactory::~ContactSearchFactory()
{
}

ContactSearchFactory::ContactSearchFactory(ContactSearchFactoryPrivate *d) :
	AbstractSearchFactory(d)
{
}

AccountItem GeneralContactSearchFactoryPrivate::addAccount(Account *account)
{
	AccountItem item(account);
	accounts.insert(item.id, item);
	QObject::connect(account, SIGNAL(destroyed()),
					 q, SLOT(accountRemoved()));
	QObject::connect(account, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
					 q, SLOT(accountStatusChanged(qutim_sdk_0_3::Status)));
	return item;
}

GeneralContactSearchFactory::GeneralContactSearchFactory(Protocol *protocol) :
	ContactSearchFactory(new GeneralContactSearchFactoryPrivate)
{
	Q_D(GeneralContactSearchFactory);
	d->q = this;
	d->protocol = protocol;
	foreach (Account *account, protocol->accounts())
		d->addAccount(account);
	connect(protocol, SIGNAL(accountCreated(qutim_sdk_0_3::Account*)),
			SLOT(accountAdded(qutim_sdk_0_3::Account*)));
}

GeneralContactSearchFactory::~GeneralContactSearchFactory()
{
}

QStringList GeneralContactSearchFactory::requestList() const
{
	Q_D(const GeneralContactSearchFactory);
	QStringList requests;
	foreach (const AccountItem &item, d->accounts) {
		if (item.isActive())
			requests << item.id;
	}
	return requests;
}

QVariant GeneralContactSearchFactory::data(const QString &request, int role)
{
	Q_D(const GeneralContactSearchFactory);
	if (role == Qt::DisplayRole || role == Qt::DecorationRole) {
		AccountsHash::const_iterator itr = d->accounts.find(request);
		if (itr != d->accounts.end() && itr->isActive()) {
			if (role == Qt::DisplayRole)
				return itr->id;
			else if (role == Qt::DecorationRole)
				return itr->status.icon();
		}
	}
	return QVariant();
}

Account *GeneralContactSearchFactory::account(const QString &name) const
{
	Q_D(const GeneralContactSearchFactory);
	AccountsHash::const_iterator itr = d->accounts.find(name);
	if (itr != d->accounts.end())
		return itr->isActive() ? itr->account : 0;
	return 0;
}

Protocol *GeneralContactSearchFactory::protocol() const
{
	return d_func()->protocol;
}

void GeneralContactSearchFactory::accountAdded(qutim_sdk_0_3::Account *account)
{
	Q_D(GeneralContactSearchFactory);
	const AccountItem &item = d->addAccount(account);
	if (item.isActive())
		emit requestAdded(account->id());
}

void GeneralContactSearchFactory::accountRemoved()
{
	Q_D(GeneralContactSearchFactory);
	Account *account = reinterpret_cast<Account*>(sender());
	AccountsHash::iterator itr = d->accounts.begin();
	AccountsHash::iterator endItr = d->accounts.end();
	while (itr != endItr) {
		if (itr->account == account) {
			QString name = itr->id;
			d->accounts.erase(itr);
			if (itr->isActive())
				emit requestRemoved(name);
			break;
		}
		++itr;
	}
}

void GeneralContactSearchFactory::accountStatusChanged(const qutim_sdk_0_3::Status &status)
{
	Q_D(GeneralContactSearchFactory);
	Q_ASSERT(qobject_cast<Account*>(sender()));
	Account *account = reinterpret_cast<Account*>(sender());
	AccountsHash::iterator itr = d->accounts.find(account->id());
	if (itr != d->accounts.end()) {
		bool active = itr->isActive();
		itr->status = status;
		if (active && status == Status::Offline)
			emit requestRemoved(itr->id);
		else if (!active && status != Status::Offline)
			emit requestAdded(itr->id);
		else
			emit requestUpdated(itr->id);
	}
}
}


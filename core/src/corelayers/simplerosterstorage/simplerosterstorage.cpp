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

#include "simplerosterstorage.h"
#include <qutim/account.h>
#include <qutim/contact.h>
#include <qutim/debug.h>

namespace Core
{
using namespace qutim_sdk_0_3;

SimpleRosterStorage::SimpleRosterStorage()
{
}

SimpleRosterStorage::~SimpleRosterStorage()
{
}

// TODO: Delayed saving

QString SimpleRosterStorage::load(Account *account)
{
	ContactsFactory *factory = account->contactsFactory();
	AccountContext &context = m_contexts[account];
	Q_ASSERT(factory);
	Config cfg = account->config();
	cfg.beginGroup(QStringLiteral("roster"));
	QString version = cfg.value(QStringLiteral("version"), QString());

	const QString contactsName = QStringLiteral("contacts");
	const QString idName = QStringLiteral("id");
	const QString dataName = QStringLiteral("data");

	{
		QVariantList contacts = cfg.value(contactsName, QVariantList());

		contacts.erase(std::remove_if(contacts.begin(), contacts.end(), [&idName] (const QVariant &data) {
			QVariantMap map = data.toMap();
			const QString id = map.value(idName).toString();
			return id.isEmpty();
		}), contacts.end());

		cfg.setValue(contactsName, contacts);
	}

	int size = cfg.beginArray(contactsName);

	for (int i = 0; i < size; i++) {
		cfg.setArrayIndex(i);
		const QString id = cfg.value(idName, QString());
		const QVariantMap data = cfg.value(dataName, QVariantMap());
		if (id.isEmpty()) {
			context.freeIndexes.append(i);
		} else {
			Contact *contact = factory->addContact(id, data);
			context.indexes.insert(contact, i);
		}
	}
	return version;
}

void SimpleRosterStorage::addContact(Contact *contact, const QString &version)
{
	Account *account = contact->account();
	ContactsFactory *factory = account->contactsFactory();
	AccountContext &context = m_contexts[account];
	Q_ASSERT(factory);
	Q_ASSERT(!context.indexes.contains(contact));
	Config cfg = account->config();
	cfg.beginGroup(QLatin1String("roster"));
	cfg.setValue(QLatin1String("version"), version);
	int size = cfg.beginArray(QLatin1String("contacts"));
	int index = context.freeIndexes.isEmpty() ? size : context.freeIndexes.takeLast();
	context.indexes.insert(contact, index);
	cfg.setArrayIndex(index);
	cfg.setValue(QLatin1String("id"), contact->id());
	QVariantMap data = cfg.value(QLatin1String("data"), QVariantMap());
	factory->serialize(contact, data);
	cfg.setValue(QLatin1String("data"), data);
}

void SimpleRosterStorage::updateContact(Contact *contact, const QString &version)
{
	Account *account = contact->account();
	ContactsFactory *factory = account->contactsFactory();
	AccountContext &context = m_contexts[account];
	Q_ASSERT(factory);
	Q_ASSERT(context.indexes.contains(contact));
	Config cfg = account->config();
	cfg.beginGroup(QLatin1String("roster"));
	cfg.setValue(QLatin1String("version"), version);
	cfg.beginArray(QLatin1String("contacts"));
	cfg.setArrayIndex(context.indexes.value(contact));
	QVariantMap data = cfg.value(QLatin1String("data"), QVariantMap());
	factory->serialize(contact, data);
	cfg.setValue(QLatin1String("data"), data);
}

void SimpleRosterStorage::removeContact(Contact *contact, const QString &version)
{
	Account *account = contact->account();
	ContactsFactory *factory = account->contactsFactory();
	AccountContext &context = m_contexts[account];
	Q_ASSERT(factory);
	Q_ASSERT(context.indexes.contains(contact));
	Config cfg = account->config();
	cfg.beginGroup(QLatin1String("roster"));
	cfg.setValue(QLatin1String("version"), version);
	cfg.beginArray(QLatin1String("contacts"));
	int index = context.indexes.take(contact);
	cfg.setArrayIndex(index);
	cfg.remove(QLatin1String("id"));
	cfg.remove(QLatin1String("data"));
	context.freeIndexes.append(index);
}
}


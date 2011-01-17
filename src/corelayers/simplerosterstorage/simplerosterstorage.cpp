/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2011 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#include "simplerosterstorage.h"
#include <qutim/account.h>
#include <qutim/contact.h>

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
	cfg.beginGroup(QLatin1String("roster"));
	QString version = cfg.value(QLatin1String("version"), QString());
	int size = cfg.beginArray(QLatin1String("contacts"));
	QString idName = QLatin1String("id");
	QString dataName = QLatin1String("data");
	for (int i = 0; i < size; i++) {
		cfg.setArrayIndex(i);
		QString id = cfg.value(idName, QString());
		QVariantMap data = cfg.value(dataName, QVariantMap());
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

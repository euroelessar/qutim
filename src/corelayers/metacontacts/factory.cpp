/****************************************************************************
 *  factory.cpp
 *
 *  Copyright (c) 2011 by Sidorov Aleksey <sauron@citadelspb.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include "factory.h"
#include <qutim/contact.h>
#include "metacontactimpl.h"
#include <qutim/account.h>
#include <qutim/protocol.h>
#include <qutim/debug.h>
#include "manager.h"

namespace Core {
namespace MetaContacts {

using namespace qutim_sdk_0_3;
	
Factory::Factory(Manager *manager) : m_manager(manager)
{
}

Factory::~Factory()
{

}

Contact* Factory::addContact(const QString& id, const QVariantMap& data)
{
	QList<Contact*> contacts;
	QVariantList items = data.value(QLatin1String("items")).toList();
	foreach(QVariant item,items) {
		QVariantMap map = item.toMap();
		Protocol *proto = Protocol::all().value(map.value(QLatin1String("protocol")).toString());
		if(!proto)
			continue;
		Account *account = proto->account(map.value(QLatin1String("account")).toString());
		if(!account)
			continue;
		Contact *contact = qobject_cast<Contact*>(account->getUnit(map.value(QLatin1String("id")).toString()));
		if(!contact)
			continue;
		contacts.append(contact);
	}
	if(!contacts.count())
		return 0;

	MetaContactImpl *metaContact = new MetaContactImpl(id);
	metaContact->setContactName(data.value(QLatin1String("name")).toString());
	metaContact->setContactAvatar(data.value(QLatin1String("avatar")).toString());
	metaContact->setContactTags(data.value(QLatin1String("tags")).toStringList());
	metaContact->addContacts(contacts);

	emit m_manager->contactCreated(metaContact);
	return metaContact;
}

void Factory::serialize(Contact* contact, QVariantMap& data)
{
	debug() << Q_FUNC_INFO;
	MetaContactImpl *metaContact = static_cast<MetaContactImpl*>(contact);
	data.insert(QLatin1String("avatar"), metaContact->avatar());
	data.insert(QLatin1String("name"),metaContact->name());
	data.insert(QLatin1String("tags"),metaContact->tags());
	
	QVariantList contacts;
	foreach(Contact *contact,metaContact->contacts()) {
		QVariantMap item;
		item.insert(QLatin1String("id"),contact->id());
		item.insert(QLatin1String("account"),contact->account()->id());
		item.insert(QLatin1String("protocol"),contact->account()->protocol()->id());
		contacts.append(item);
	}
	data.insert(QLatin1String("items"),contacts);
}


} // namespace MetaContacts
} // namespace Core

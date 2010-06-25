/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#include "manager.h"
#include "modulemanagerimpl.h"
#include "libqutim/protocol.h"

namespace Core
{
	namespace MetaContacts
	{
		static CoreModuleHelper<Manager> metacontacts_static(
				QT_TRANSLATE_NOOP("Plugin", "Simple MetaContacts"),
				QT_TRANSLATE_NOOP("Plugin", "Default qutIM MetaContacts implementation, very stupid one")
				);
		
		Manager::Manager()
		{
		}
		
		Manager::~Manager()
		{
			Config cfg;
			cfg.beginGroup("metaContact");
			cfg.remove("contacts");
			cfg.beginArray("contacts");
			QList<MetaContactImpl*> contacts = m_contacts.values();
			for (int i = 0; i < contacts.size(); i++) {
				cfg.setArrayIndex(i);
				MetaContactImpl *metaContact = contacts.at(i);
				cfg.setValue("id", metaContact->id());
				cfg.setValue("name", metaContact->name());
				cfg.beginArray("subItems");
				for (int j = 0; j < metaContact->contacts().size(); j++) {
					cfg.setArrayIndex(j);
					Contact *contact = metaContact->contacts().at(j);
					cfg.setValue("id", contact->id());
					cfg.setValue("account", contact->account()->id());
					cfg.setValue("protocol", contact->account()->protocol()->id());
				}
				cfg.endArray();
			}
		}
		
		ChatUnit *Manager::getUnit(const QString &unitId, bool create)
		{
			MetaContactImpl *contact = m_contacts.value(unitId);
			if (!contact && create) {
				contact = new MetaContactImpl(unitId);
				m_contacts.insert(unitId, contact);
				emit contactCreated(contact);
			}
			return contact;
		}
		
		void Manager::loadContacts()
		{
			Config cfg;
			cfg.beginGroup("metaContact");
			int size = cfg.beginArray("contacts");
			for (int i = 0; i < size; i++) {
				cfg.setArrayIndex(i);
				QString id = cfg.value("id", QString());
				QString name = cfg.value("name", QString());
				MetaContactImpl *metaContact = 0;
				int subItemCount = cfg.beginArray("subItems");
				for (int j = 0; j < subItemCount; j++) {
					cfg.setArrayIndex(j);
					Protocol *proto = allProtocols().value(cfg.value("protocol", QString()));
					if (!proto)
						continue;
					Account *account = proto->account(cfg.value("account", QString()));
					if (!account)
						continue;
					ChatUnit *unit = account->getUnit(cfg.value("id", QString()));
					if (Contact *contact = qobject_cast<Contact*>(unit)) {
						if (!metaContact) {
							metaContact = new MetaContactImpl(id);
							metaContact->setName(name);
						}
						metaContact->addContact(contact);
					}
				}
				if (metaContact) {
					m_contacts.insert(id, metaContact);
					emit contactCreated(metaContact);
				}
				cfg.endArray();
			}
		}
}
}

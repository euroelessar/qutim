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
#include <qutim/protocol.h>
#include <qutim/debug.h>
#include <qutim/actiongenerator.h>
#include <qutim/icon.h>
#include "mergedialog.h"
#include <qutim/systemintegration.h>
#include <qutim/contactlist.h>

namespace Core
{
namespace MetaContacts
{
using namespace qutim_sdk_0_3;

Manager::Manager()
{
	ActionGenerator *gen = new ActionGenerator(Icon("list-remove-user"),
											   QT_TRANSLATE_NOOP("MetaContact","Split Metacontact"),
											   this,
											   SLOT(onSplitTriggered(QObject*)));
	gen->setType(ActionTypeContactList);
	MenuController::addAction<MetaContactImpl>(gen);
	gen = new ActionGenerator(Icon("list-add-user"),
							  QT_TRANSLATE_NOOP("MetaContact","Manage metacontacts"),
							  this,
							  SLOT(onCreateTriggered(QObject*)));
	gen->setType(ActionTypeContactList);
	MenuController::addAction<MetaContactImpl>(gen);
	MenuController::addAction<ContactList>(gen);
}

Manager::~Manager()
{
	debug() << Q_FUNC_INFO;
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
			Protocol *proto = Protocol::all().value(cfg.value("protocol", QString()));
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

void Manager::onSplitTriggered(QObject *object)
{
	//TODO implement logic
	MetaContactImpl *metaContact = qobject_cast<MetaContactImpl*>(object);
	foreach (Contact *c, metaContact->contacts()) {
		metaContact->removeContact(c);
	}
}

void Manager::onCreateTriggered(QObject *obj)
{
	MergeDialog *dialog = new MergeDialog;
	if(MetaContactImpl *m = qobject_cast<MetaContactImpl*>(obj))
		dialog->setMetaContact(m);
	dialog->setAttribute(Qt::WA_DeleteOnClose);
	centerizeWidget(dialog);
	SystemIntegration::show(dialog);
}

QString Manager::name() const
{
	//TODO implement logic
	return (QT_TRANSLATE_NOOP("Metacontact","You")).toString();
}

}

}

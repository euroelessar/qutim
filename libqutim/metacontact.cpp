/****************************************************************************
 *  metacontact.cpp
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#include "metacontact.h"
#include "metacontactmanager.h"
#include "contact_p.h"
#include <QCoreApplication>

namespace qutim_sdk_0_3
{
class MetaContactPrivate : public ContactPrivate
{
public:
	MetaContactPrivate(MetaContact *c) : ContactPrivate(c), atAddState(false) {}
	bool atAddState;
};

MetaContact::MetaContact() : Contact(*new MetaContactPrivate(this),
		MetaContactManager::instance())
{
}

MetaContact::~MetaContact()
{
}

void MetaContact::addContact(Contact *contact)
{
	Q_D(MetaContact);
	ContactPrivate *c_p = contact->d_func();
	if (c_p->metaContact == this)
		return;
	MetaContactChangeEvent ev(contact, c_p->metaContact, this);
	d->atAddState = true;
	if (c_p->metaContact)
		c_p->metaContact->removeContact(contact);
	d->atAddState = false;
	qApp->sendEvent(this, &ev);
	qApp->sendEvent(contact, &ev);
	emit contactAdded(contact);
}

void MetaContact::removeContact(Contact *contact)
{
	Q_D(MetaContact);
	ContactPrivate *c_p = contact->d_func();
	if (c_p->metaContact == this && !d->atAddState) {
		MetaContactChangeEvent ev(contact, this, 0);
		qApp->sendEvent(this, &ev);
		qApp->sendEvent(contact, &ev);
		emit contactRemoved(contact);
	}
}

bool MetaContact::isInList() const
{
	return true;
}

void MetaContact::setInList(bool inList)
{
	Q_UNUSED(inList);
}

ChatUnitList MetaContact::lowerUnits()
{
	return ChatUnitList();
}

MetaContactChangeEvent::MetaContactChangeEvent(Contact *c_, MetaContact *om_, MetaContact *nm_)
	: QEvent(MetaContactChangeEvent::eventType()), c(c_), om(om_), nm(nm_)
{
}

MetaContactChangeEvent::~MetaContactChangeEvent()
{
}

QEvent::Type MetaContactChangeEvent::eventType()
{
	static QEvent::Type type = QEvent::Type(QEvent::registerEventType(QEvent::User + 104));
	return type;
}
}

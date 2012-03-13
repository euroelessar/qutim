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
	if (c_p->metaContact.data() == this)
		return;
	MetaContact *other = c_p->metaContact.data();
	MetaContactChangeEvent ev(contact, c_p->metaContact.data(), this);
	if (other)
		other->d_func()->atAddState = true;
	if (c_p->metaContact)
		c_p->metaContact.data()->removeContact(contact);
	if (other)
		other->d_func()->atAddState = false;
	qApp->sendEvent(contact, &ev);
	qApp->sendEvent(this, &ev);
	if (other)
		emit other->contactRemoved(contact);
	emit contactAdded(contact);
}

void MetaContact::removeContact(Contact *contact)
{
	Q_D(MetaContact);
	ContactPrivate *c_p = contact->d_func();
	if (c_p->metaContact.data() == this && !d->atAddState) {
		MetaContactChangeEvent ev(contact, this, 0);
		qApp->sendEvent(contact, &ev);
		qApp->sendEvent(this, &ev);
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


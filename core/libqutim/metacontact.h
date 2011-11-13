/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef METACONTACT_H
#define METACONTACT_H

#include "contact.h"

namespace qutim_sdk_0_3
{
class MetaContactPrivate;

class LIBQUTIM_EXPORT MetaContact : public Contact
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(MetaContact)
public:
	MetaContact();
	virtual ~MetaContact();

	virtual bool isInList() const;
	virtual void setInList(bool inList);

	virtual void addContact(Contact *contact);
	virtual void removeContact(Contact *contact);

	virtual ChatUnitList lowerUnits();
	//		virtual const ChatUnit *getHistoryUnit() const;
signals:
	void contactAdded(qutim_sdk_0_3::Contact *);
	void contactRemoved(qutim_sdk_0_3::Contact *);
};

class LIBQUTIM_EXPORT MetaContactChangeEvent : public QEvent
{
public:
	MetaContactChangeEvent(Contact *contact, MetaContact *oldMetaContact, MetaContact *newMetaContact);
	~MetaContactChangeEvent();
	static QEvent::Type eventType();
	Contact *contact() const { return c; }
	MetaContact *oldMetaContact() const { return om; }
	MetaContact *newMetaContact() const { return nm; }
private:
	Contact *c;
	MetaContact *om;
	MetaContact *nm;
};
}

#endif // METACONTACT_H


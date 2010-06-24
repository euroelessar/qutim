/****************************************************************************
 *  metacontact.h
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

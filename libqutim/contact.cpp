/****************************************************************************
 *  contact.cpp
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

#include "contact.h"
#include "chatunit_p.h"
#include "account.h"
#include <libqutim/icon.h>

namespace qutim_sdk_0_3
{
	class ContactPrivate : public ChatUnitPrivate
	{
	public:
		ContactPrivate(Contact *c) : ChatUnitPrivate(c) {}
	};

	Contact::Contact(Account *account) : ChatUnit(*new ContactPrivate(this), account)
	{
	}

	Contact::~Contact()
	{
//		if(Account *account = qobject_cast<Account *>(parent()))
//		{
//			ConfigGroup data = account->config("contacts").group(id()).group("data");
//			foreach(const QByteArray &name, dynamicPropertyNames())
//				data.setValue(name, property(name));
//			data.sync();
//		}
	}

	QString Contact::title() const
	{
		QString n = name();
		return n.isEmpty() ? id() : n;
	}

	QSet<QString> Contact::tags() const
	{
		return QSet<QString>();
	}

	QString Contact::name() const
	{
		return id();
	}

	Status Contact::status() const
	{
		return Online;
	}

	QIcon Contact::statusIcon() const
	{
		switch(status())
		{
		default:
			if(status() >= Connecting)
				return Icon("network-connect");
		case Online:
		case AtHome:
		case FreeChat:
		case Invisible:
			return Icon("user-online");
		case Offline:
			return Icon("user-offline");
		case Away:
		case OutToLunch:
			return Icon("user-away");
		case DND:
		case Evil:
		case Depression:
		case Occupied:
		case AtWork:
		case OnThePhone:
			return Icon("user-busy");
		case NA:
			return Icon("user-away-extended");
		}
	}
}

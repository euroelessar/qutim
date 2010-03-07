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
#include "buddy_p.h"
#include "account.h"
#include <libqutim/icon.h>

namespace qutim_sdk_0_3
{
	class ContactPrivate : public BuddyPrivate
	{
	public:
		ContactPrivate(Contact *c) : BuddyPrivate(c) {}
	};

	Contact::Contact(Account *account) : Buddy(*new ContactPrivate(this), account)
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

	QSet<QString> Contact::tags() const
	{
		return QSet<QString>();
	}
}

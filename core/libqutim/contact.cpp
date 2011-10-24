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

#include "contact_p.h"
#include "account.h"
#include "icon.h"
#include "metacontact.h"

namespace qutim_sdk_0_3
{
Contact::Contact(Account *account) : Buddy(*new ContactPrivate(this), account)
{
}

Contact::Contact(ContactPrivate &d, Account *account) : Buddy(d, account)
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

QStringList Contact::tags() const
{
	return QStringList();
}

ChatUnit *Contact::upperUnit()
{
	return d_func()->metaContact.data();
}

bool Contact::event(QEvent *e)
{
	if (e->type() == MetaContactChangeEvent::eventType()) {
		MetaContactChangeEvent *metaEvent = static_cast<MetaContactChangeEvent*>(e);
		d_func()->metaContact = metaEvent->newMetaContact();
	}
	return Buddy::event(e);
}
}

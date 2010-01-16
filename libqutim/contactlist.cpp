/****************************************************************************
 *  contactlist.cpp
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

#include "contactlist.h"
#include "contact.h"
#include "account.h"
#include "objectgenerator.h"
#include <QPointer>

namespace qutim_sdk_0_3
{
	ContactList *ContactList::instance()
	{
		static QPointer<ContactList> self;
		if(self.isNull() && isCoreInited())
		{
			GeneratorList exts = moduleGenerators<ContactList>();
			self = exts.first()->generate<ContactList>();
		}
		return self;
	}

	ContactList::ContactList()
	{
	}

	ContactList::~ContactList()
	{
	}

	void ContactList::virtual_hook(int id, void *data)
	{
		Q_UNUSED(id);
		Q_UNUSED(data);
	}
}

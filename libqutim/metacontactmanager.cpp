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

#include "metacontactmanager.h"
#include "metacontactprotocol_p.h"
#include "account_p.h"
#include "metacontact.h"
#include <QUuid>

namespace qutim_sdk_0_3
{
	class MetaContactManagerPrivate : public AccountPrivate
	{
	public:
		MetaContactManagerPrivate(Account *a) : AccountPrivate(a) {}
	};
	
	MetaContactManager::MetaContactManager() 
		: Account(*new MetaContactManagerPrivate(this), new MetaProtocol(this))
	{
		Q_D(MetaContactManager);
		d->id = QLatin1String("meta");
	}
	
	MetaContactManager::~MetaContactManager()
	{
	}

	MetaContact *MetaContactManager::createContact()
	{
		QString id = QUuid::createUuid().toString();
		MetaContact *contact = qobject_cast<MetaContact*>(getUnit(id, true));
		Q_ASSERT(contact);
		return contact;
	}
	
	MetaContactManager *MetaContactManager::instance()
	{
		static MetaContactManager *self = 0;
		if (!self && isCoreInited())
			self = qobject_cast<MetaContactManager*>(getService("MetaContactManager"));
		return self;
	}
	
	bool MetaContactManager::eventFilter(QObject *obj, QEvent *ev)
	{
		return Account::eventFilter(obj, ev);
	}
}

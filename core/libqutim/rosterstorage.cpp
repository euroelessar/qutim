/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2011 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#include "rosterstorage.h"
#include "objectgenerator.h"
#include "servicemanager.h"
#include <QPointer>

namespace qutim_sdk_0_3
{

class RosterStoragePrivate
{
public:
};

ContactsFactory::~ContactsFactory()
{
}

class FakeRosterStorage : public RosterStorage
{
public:
	FakeRosterStorage() {}

	virtual QString load(Account *) { return QString(); }
	virtual void addContact(Contact *, const QString &) {}
	virtual void updateContact(Contact *, const QString &) {}
	virtual void removeContact(Contact *, const QString &) {}
};

RosterStorage *RosterStorage::instance()
{
	static RosterStorage *self = NULL;
	if (!self && ObjectGenerator::isInited()) {
		self = ServiceManager::getByName<RosterStorage*>("RosterStorage");
		if (!self)
			self = new FakeRosterStorage;
	}
	return self;
}

RosterStorage::RosterStorage() : d_ptr(new RosterStoragePrivate)
{
}

RosterStorage::~RosterStorage()
{
}
}

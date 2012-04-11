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

#include "rosterstorage.h"
#include "objectgenerator.h"
#include "servicemanager.h"
#include <QWeakPointer>

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


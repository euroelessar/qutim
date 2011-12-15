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

#include "abstractcontact.h"
#include "icon.h"

namespace qutim_sdk_0_3
{
struct AbstractContactPrivate
{
	QString id;
};

AbstractContact::AbstractContact(const QString &id, Account *parent) : ChatUnit(parent), p(new AbstractContactPrivate)
{
	p->id = id;
}

AbstractContact::~AbstractContact()
{
}

QString AbstractContact::id()
{
	return p->id;
}

QString AbstractContact::name() const
{
	return p->id;
}

Status AbstractContact::status() const
{
	return Status::Online;
}

MetaContact *AbstractContact::metaContact()
{
	return 0;
}
}


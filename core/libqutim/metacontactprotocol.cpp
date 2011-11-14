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

#include "metacontactprotocol_p.h"
#include "metacontactmanager.h"

namespace qutim_sdk_0_3
{
	MetaProtocol::MetaProtocol(MetaContactManager *manager) : Protocol(*new MetaProtocolPrivate)
	{
		d_func()->manager = manager;
	}
	
	MetaProtocol::~MetaProtocol()
	{
	}
	
	QList<Account *> MetaProtocol::accounts() const
	{
		Q_D(const MetaProtocol);
		QList<Account*> list;
		list << d->manager;
		return list;
	}
	
	Account *MetaProtocol::account(const QString &id) const
	{
		Q_D(const MetaProtocol);
		if (id == QLatin1String("meta"))
			return d->manager;
		return 0;
	}
	
	void MetaProtocol::loadAccounts()
	{
	}
}


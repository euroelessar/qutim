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

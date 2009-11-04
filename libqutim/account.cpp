/****************************************************************************
 *  account.cpp
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#include <QPointer>
#include "account.h"
#include "contact.h"
#include "protocol.h"

namespace qutim_sdk_0_3
{
	struct AccountPrivate
	{
		QPointer<Protocol> protocol;
		QString id;
		Status status;
	};

	Account::Account(const QString &id, Protocol *protocol) : QObject(protocol), p(new AccountPrivate)
	{
		p->protocol = protocol;
		p->id = id;
		p->status = Offline;
	}

	Account::~Account()
	{
//		ConfigGroup data = config("properties");
//		foreach(const QByteArray &name, dynamicPropertyNames())
//			data.setValue(name, property(name));
//		data.sync();
	}

	QString Account::id() const
	{
		return p->id;
	}

	Config Account::config()
	{
		return Config(p->protocol->id() + QLatin1Char('.') + p->id + QLatin1String("/account"));
	}

	ConfigGroup Account::config(const QString &group)
	{
		return config().group(group);
	}
		
	Status Account::status() const
	{
		return p->status;
	}

	Protocol *Account::protocol()
	{
		return p->protocol;
	}

	const Protocol *Account::protocol() const
	{
		return p->protocol;
	}
		
	void Account::setStatus(Status status)
	{
		p->status = status;
	}
}

/****************************************************************************
 *  account.cpp
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

#include <QPointer>
#include <QStringBuilder>
#include "menucontroller_p.h"
#include "account.h"
#include "contact.h"
#include "protocol.h"
#include "status.h"

namespace qutim_sdk_0_3
{
	class AccountPrivate : public MenuControllerPrivate
	{
	public:
		AccountPrivate(Account *a) : MenuControllerPrivate(a) {}
		QPointer<Protocol> protocol;
		QString id;
		Status status;
	};

	Account::Account(const QString &id, Protocol *protocol) : MenuController(*new AccountPrivate(this), protocol)
	{
		Q_D(Account);
		d->protocol = protocol;
		d->id = id;
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
		return d_func()->id;
	}

	QString Account::name() const
	{
		return d_func()->id;
	}

	Config Account::config()
	{
		Q_D(Account);
		return Config(d->protocol->id() % QLatin1Char('.') % d->id % QLatin1Literal("/account"));
	}

	ConfigGroup Account::config(const QString &name)
	{
		return config().group(name);
	}
		
	Status Account::status() const
	{
		return d_func()->status;
	}

	Protocol *Account::protocol()
	{
		return d_func()->protocol;
	}

	const Protocol *Account::protocol() const
	{
		return d_func()->protocol;
	}
		
	void Account::setStatus(Status status)
	{
		d_func()->status = status;
	}

	ChatUnit *Account::getUnitForSession(ChatUnit *unit)
	{
		return unit;
	}

	InfoRequest *Account::infoRequest() const
	{
		return 0;
	}
}

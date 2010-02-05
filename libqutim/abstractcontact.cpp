/****************************************************************************
 *  abstractcontact.cpp
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
		return Online;
	}

	QIcon AbstractContact::statusIcon() const
	{
		switch(status())
		{
		default:
			if(status() >= Connecting)
				return Icon("network-connect");
		case Online:
		case AtHome:
		case FreeChat:
		case Invisible:
			return Icon("user-online");
		case Offline:
			return Icon("user-offline");
		case Away:
		case OutToLunch:
			return Icon("user-away");
		case DND:
		case Evil:
		case Depression:
		case Occupied:
		case AtWork:
		case OnThePhone:
			return Icon("user-busy");
		case NA:
			return Icon("user-away-extended");
		}
	}

	MetaContact *AbstractContact::metaContact()
	{
		return 0;
	}
}

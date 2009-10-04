/****************************************************************************
 *  contact.cpp
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

#include "contact.h"
#include "account.h"
#include <libqutim/icon.h>

namespace qutim_sdk_0_3
{
	Contact::Contact(Account *account)
			: QObject(account), m_type(Simple), m_account(account)
	{
	}

	QSet<QString> Contact::tags() const
	{
		return QSet<QString>();
	}

	QString Contact::name() const
	{
		return id();
	}

	Status Contact::status() const
	{
		return Online;
	}

	QIcon Contact::statusIcon() const
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
}

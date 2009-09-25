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

namespace qutim_sdk_0_3
{
	Contact::Contact(Account *account)
			: QObject(account), m_type(Simple), m_account(account)
	{
	}

	QString Contact::name() const
	{
		return id();
	}
}

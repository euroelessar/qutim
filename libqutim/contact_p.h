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

#ifndef CONTACT_P_H
#define CONTACT_P_H

#include "contact.h"
#include "buddy_p.h"

namespace qutim_sdk_0_3
{
	class ContactPrivate : public BuddyPrivate
	{
	public:
		ContactPrivate(Contact *c) : BuddyPrivate(c) {}
		QPointer<MetaContact> metaContact;
	};
}
#endif // CONTACT_P_H

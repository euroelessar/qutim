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

#include "buddyadapter.h"

BuddyAdapter::BuddyAdapter(Buddy *buddy) :
		QDBusAbstractAdaptor(buddy)
{
	connect(buddy, SIGNAL(nameChanged(QString)), this, SIGNAL(nameChanged(QString)));
	connect(buddy, SIGNAL(avatarChanged(QString)), this, SIGNAL(nameChanged(QString)));
	connect(buddy, SIGNAL(statusChanged(qutim_sdk_0_3::Status)),
			this, SIGNAL(statusChanged(qutim_sdk_0_3::Status)));
}

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

#ifndef ACCOUNT_P_H
#define ACCOUNT_P_H

#include "menucontroller_p.h"
#include "account.h"
#include "protocol.h"
#include "status.h"
#include "groupchatmanager_p.h"

namespace qutim_sdk_0_3
{
	class AccountPrivate : public MenuControllerPrivate
	{
	public:
		AccountPrivate(Account *a) : MenuControllerPrivate(a) {}
		QPointer<Protocol> protocol;
		QString id;
		Status status;
		GroupChatManager *groupChatManager;
	};
}

#endif // ACCOUNT_P_H

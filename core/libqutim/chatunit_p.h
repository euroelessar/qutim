/****************************************************************************
 *  chatunit_p.h
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

#ifndef CHATUNIT_P_H
#define CHATUNIT_P_H

#include "chatunit.h"
#include "menucontroller_p.h"
#include "notification.h"

namespace qutim_sdk_0_3
{
	class ChatUnitPrivate : public MenuControllerPrivate
	{
	public:
		ChatUnitPrivate(ChatUnit *u) : MenuControllerPrivate(u),account(0),chatState(ChatStateInActive) {}
		Account *account;
		ChatState chatState;
		QWeakPointer<Notification> composingNotification;
	};
}

#endif // CHATUNIT_P_H

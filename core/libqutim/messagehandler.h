/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2011 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include "message.h"

namespace qutim_sdk_0_3
{

class LIBQUTIM_EXPORT MessageHandler
{
public:
	enum Result
	{
		Accept,
		Reject,
		Error
	};
	enum Priority
	{
		LowPriority       = 0x00000100,
		ChatInPriority    = 0x00008000,
		NormalPriortity   = 0x00010000,
		ChatOutPriority   = 0x00020000,
		HighPriority      = 0x01000000
	};

	virtual ~MessageHandler();
	
	static void registerHandler(MessageHandler *handler,
	                            int incomingPriority = NormalPriortity,
	                            int outgoingPriority = NormalPriortity);
	static void unregisterHandler(MessageHandler *handler);
	static Result handle(Message &message, QString *reason = 0);
	
protected:
	virtual Result doHandle(Message &message, QString *reason) = 0;
};

}

#endif // MESSAGEHANDLER_H

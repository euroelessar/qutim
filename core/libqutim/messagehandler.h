/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
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
		HighPriority      = 0x01000000,
		SenderPriority    = 0x02000000
	};

	virtual ~MessageHandler();
	
	static void registerHandler(MessageHandler *handler,
	                            int incomingPriority = NormalPriortity,
	                            int outgoingPriority = NormalPriortity);
	static void registerHandler(MessageHandler *handler, const QString &name,
	                            int incomingPriority = NormalPriortity,
	                            int outgoingPriority = NormalPriortity);
	static void unregisterHandler(MessageHandler *handler);
	static Result handle(Message &message, QString *reason = 0);
	static void traceHandlers();
	
protected:
	virtual Result doHandle(Message &message, QString *reason) = 0;
};

}

#endif // MESSAGEHANDLER_H


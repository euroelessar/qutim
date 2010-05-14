/****************************************************************************
 *  messagemodifier.h
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
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

#ifndef ADIUMMESSAGEMODIFIER_H
#define MESSAGEMODIFIER_H

#include <QObject>
#include "chatsessionimpl.h"

namespace qutim_sdk_0_3
{
	class ChatSession;
	class Message;
}
namespace Core
{
	namespace AdiumChat
	{
		class MessageModifier
		{
		public:
			virtual ~MessageModifier() {}
			virtual QString getValue(const ChatSession *session, const Message &message, const QString &name, const QString &value) = 0;
			virtual QStringList supportedNames() const = 0;
		};
	}
}
Q_DECLARE_INTERFACE(Core::AdiumChat::MessageModifier, "org.qutim.AdiumChat.MessageModifier")

#endif // ADIUMMESSAGEMODIFIER_H

/****************************************************************************
 *  quetzalchatuser.cpp
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

#include "quetzalchatuser.h"
#include "quetzalchat.h"


QuetzalChatUser::QuetzalChatUser(PurpleConvChatBuddy *user, QuetzalChat *chat) :
	Buddy(chat->account())
{
	m_user = user;
	m_id = user->name;
	m_name = user->alias;
}

void QuetzalChatUser::sendMessage(const Message &message)
{
}

Status QuetzalChatUser::status() const
{
	return Status(Status::Online);
}

void QuetzalChatUser::update()
{
}

void QuetzalChatUser::rename(const QString &id, const QString &name)
{
	QString old_title = title();
	m_id = id;
	m_name = name;
	if (old_title != title()) {
		titleChanged(title());
	}
}

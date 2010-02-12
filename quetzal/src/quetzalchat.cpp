/****************************************************************************
 *  quetzalchat.cpp
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

#include "quetzalchat.h"
#include <qutim/messagesession.h>

QuetzalChat::QuetzalChat(PurpleConversation *conv) :
	QuetzalConversation(conv)
{
}

void QuetzalChat::addUsers(GList *cbuddies, gboolean new_arrivals)
{
	for (; cbuddies; cbuddies = cbuddies->next) {
		PurpleConvChatBuddy *buddy = reinterpret_cast<PurpleConvChatBuddy *>(cbuddies->data);
		QuetzalChatUser *user = new QuetzalChatUser(buddy, this);
		m_users.insert(user->id(), user);
		ChatLayer::get(this, true)->addContact(user);
	}
}

void QuetzalChat::renameUser(const char *old_name, const char *new_name, const char *new_alias)
{
	QuetzalChatUser *user = m_users.take(old_name);
	QString id = new_name;
	m_users.insert(id, user);
	user->rename(id, new_alias);
}

void QuetzalChat::removeUsers(GList *users)
{
	for (; users; users = users->next) {
		const char *name = reinterpret_cast<const char *>(users->data);
		QuetzalChatUser *user = m_users.take(name);
		ChatLayer::get(this, true)->removeContact(user);
		user->deleteLater();
	}
}

void QuetzalChat::updateUser(const char *user)
{
	m_users.value(user)->update();
}

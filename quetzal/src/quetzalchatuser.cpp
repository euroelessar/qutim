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
	m_conv = chat->purple();
	m_name = user->alias;
	fixId();
}

void QuetzalChatUser::fixId()
{
	PurpleConnection *gc = m_conv->account->gc;
	PurplePluginProtocolInfo *info = PURPLE_PLUGIN_PROTOCOL_INFO(gc->prpl);
	if (!qstrcmp(m_conv->account->protocol_id, "prpl-jabber")) {
		m_id = m_conv->name;
		m_id += QLatin1Char('/');
		m_id += m_user->name;
	} else if (info->get_cb_real_name) {
		char *id = info->get_cb_real_name(gc, PURPLE_CONV_CHAT(m_conv)->id, m_user->name);
		m_id = id;
		g_free(id);
	} else {
		m_id = m_user->name;
	}
}

bool QuetzalChatUser::sendMessage(const Message &message)
{
	return false;
}

Status QuetzalChatUser::status() const
{
	return Status(Status::Online);
}

void QuetzalChatUser::update()
{
}

void QuetzalChatUser::rename(const QString &name)
{
	QString old_title = title();
	fixId();
	m_name = name;
	if (old_title != title()) {
		titleChanged(title(), old_title);
	}
}

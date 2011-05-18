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
	m_conv = chat->purple();
	m_name = user->alias;
	fixId(user);
}

void QuetzalChatUser::fixId(PurpleConvChatBuddy *user)
{
	m_user = user;
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

extern bool quetzal_send_message(PurpleAccount *account, Buddy *buddy, const Message &message);

bool QuetzalChatUser::sendMessage(const Message &message)
{
	return quetzal_send_message(m_conv->account, this, message);
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
	m_name = name;
	if (old_title != title()) {
		titleChanged(title(), old_title);
	}
}

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


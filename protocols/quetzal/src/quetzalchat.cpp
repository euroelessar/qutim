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

#include "quetzalchat.h"
#include "quetzalaccount.h"
#include <qutim/chatsession.h>
#include <qutim/debug.h>

QuetzalChat::QuetzalChat(PurpleConversation *conv) : 
		Conference(reinterpret_cast<Account *>(conv->account->ui_data))
{
	m_conv = conv;
	m_conv->ui_data = this;
	m_id = m_conv->name;
	m_title = m_conv->title;
	if (m_id.isEmpty()) {
		// Hack for msn-like protocols
		m_id = QLatin1String("QuetzalChat#")
			   + QString::number(quint64(qrand()) << 20 | quint64(qrand()), 16);
	}
	PurpleConvChat *chat = purple_conversation_get_chat_data(conv);
	if (!chat->left)
		setJoined(true);
//	if (account()->protocol()->id() == "msn")
//		m_id = "QuetzalChat#" + QString::number(chat->id);
}

QString QuetzalChat::id() const
{
	return m_id;
}

QString QuetzalChat::title() const
{
	return m_title;
}

QString QuetzalChat::topic() const
{
	return m_topic;
}

void QuetzalChat::setTopic(const QString &topic)
{
	PurpleConnection *gc = m_conv->account->gc;
	if (!gc)
		return;
	PurplePluginProtocolInfo *info = PURPLE_PLUGIN_PROTOCOL_INFO(gc->prpl);
	if (!info->set_chat_topic)
		return;
	info->set_chat_topic(gc, PURPLE_CONV_CHAT(m_conv)->id, topic.toUtf8().constData());
}

void QuetzalChat::addUsers(GList *cbuddies, gboolean new_arrivals)
{
	Q_UNUSED(new_arrivals);
	QuetzalAccount *account = reinterpret_cast<QuetzalAccount*>(m_conv->account->ui_data);
	for (; cbuddies; cbuddies = cbuddies->next) {
		PurpleConvChatBuddy *buddy = reinterpret_cast<PurpleConvChatBuddy *>(cbuddies->data);
		QuetzalChatUser *user = new QuetzalChatUser(buddy, this);
		account->addChatUnit(user);
		m_users.insert(buddy->name, user);
		ChatLayer::get(this, true)->addContact(user);
	}
}

Buddy *QuetzalChat::me() const
{
	PurpleConvChat *data = purple_conversation_get_chat_data(m_conv);
	return m_users.value(m_nick.isEmpty() ? QString::fromUtf8(data->nick) : m_nick);
}

void QuetzalChat::setMe(const char *nick)
{
	m_nick = nick;
}

void QuetzalChat::doJoin()
{
	PurpleConnection *gc = m_conv->account->gc;
	if (!gc)
		return;
	PurplePluginProtocolInfo *info = PURPLE_PLUGIN_PROTOCOL_INFO(gc->prpl);
	if (!info->join_chat || !info->chat_info_defaults)
		return;
	GHashTable *comps = info->chat_info_defaults(gc, m_id.toUtf8().constData());
	info->join_chat(gc, comps);
	g_hash_table_destroy(comps);
}

void QuetzalChat::doLeave()
{
	PurpleConnection *gc = m_conv->account->gc;
	if (!gc)
		return;
	PurplePluginProtocolInfo *info = PURPLE_PLUGIN_PROTOCOL_INFO(gc->prpl);
	if (!info->chat_leave)
		return;
	info->chat_leave(gc, PURPLE_CONV_CHAT(m_conv)->id);
}

void QuetzalChat::renameUser(const char *old_name, const char *new_name, const char *new_alias)
{
	QuetzalAccount *account = reinterpret_cast<QuetzalAccount*>(m_conv->account->ui_data);
	PurpleConvChat *data = purple_conversation_get_chat_data(m_conv);
	QuetzalChatUser *user = m_users.take(old_name);
	account->removeChatUnit(user);
	user->fixId(purple_conv_chat_cb_find(data, new_name));
	m_users.insert(new_name, user);
	user->rename(new_alias);
	account->addChatUnit(user);
//	if (!qstrcmp(old_name, data->nick))
//		emit meChanged(user);
}

void QuetzalChat::removeUsers(GList *users)
{
	QuetzalAccount *account = reinterpret_cast<QuetzalAccount*>(m_conv->account->ui_data);
	for (; users; users = users->next) {
		const char *name = reinterpret_cast<const char *>(users->data);
		QuetzalChatUser *user = m_users.take(name);
		account->removeChatUnit(user);
		user->deleteLater();
		ChatLayer::get(this, true)->removeContact(user);
	}
}

void QuetzalChat::updateUser(const char *user)
{
	m_users.value(user)->update();
}

bool QuetzalChat::sendMessage(const Message &message)
{
	if (!m_conv->account->gc)
		return false;
	purple_conv_chat_send(PURPLE_CONV_CHAT(m_conv), message.text().toUtf8().constData());
	return true;
}

void QuetzalChat::update(PurpleConvUpdateType type)
{
	PurpleConvChat *chat = PURPLE_CONV_CHAT(m_conv);
	if (type == PURPLE_CONV_UPDATE_CHATLEFT)
		setJoined(chat->left);
	else if (type == PURPLE_CONV_UPDATE_TOPIC) {
		QString oldTopic = m_topic;
		m_topic = purple_conv_chat_get_topic(PURPLE_CONV_CHAT(m_conv));
		emit topicChanged(m_topic, oldTopic);
	} else if (type == PURPLE_CONV_UPDATE_TITLE) {
//		QString oldTitle = m_title;
//		m_title = purple_PURPLE_CONV_CHAT(m_conv));
//		emit topicChanged(m_topic, oldTopic);
	}
}

void QuetzalChat::invite(qutim_sdk_0_3::Contact *contact, const QString &reason)
{
	QByteArray who = contact->id().toUtf8();
	purple_conv_chat_invite_user(PURPLE_CONV_CHAT(m_conv), who.constData(),
								 reason.toUtf8().constData(), false);
}


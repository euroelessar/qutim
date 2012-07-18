/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#include "vroster.h"
#include "vcontact.h"
#include "vaccount.h"
#include "vgroupchat.h"

#include <vk/roster.h>
#include <vk/longpoll.h>
#include <vk/message.h>

#include <qutim/chatsession.h>

using namespace qutim_sdk_0_3;

VRoster::VRoster(VAccount *account) : QObject(account),
	m_account(account)
{
	connect(m_account->client()->roster(), SIGNAL(friendAdded(vk::Buddy*)), SLOT(onAddFriend(vk::Buddy*)));
	connect(m_account->client(), SIGNAL(onlineStateChanged(bool)), SLOT(onOnlineChanged(bool)));

	vk::LongPoll *poll = m_account->client()->longPoll();
	connect(poll, SIGNAL(messageAdded(vk::Message)), SLOT(onMessageAdded(vk::Message)));
	connect(poll, SIGNAL(contactTyping(int, int)), SLOT(onContactTyping(int, int)));
}

VContact *VRoster::contact(int id, bool create)
{
	VContact *c = m_contactHash.value(id);
	if (!c && create && id != m_account->uid()) {
		vk::Buddy *buddy = m_account->client()->roster()->buddy(id);
		c = createContact(buddy);
	}
	return c;
}

VContact *VRoster::contact(int id) const
{
	return m_contactHash.value(id);
}

VGroupChat *VRoster::groupChat(int id, bool create)
{
	VGroupChat *c = m_groupChatHash.value(id);
	if (!c && create) {
		c = new VGroupChat(m_account, id);
		connect(c, SIGNAL(destroyed(QObject*)), SLOT(onGroupChatDestroyed(QObject*)));
		m_groupChatHash.insert(id, c);
	}
	return c;
}

VGroupChat *VRoster::groupChat(int id) const
{
	return m_groupChatHash.value(id);
}

VContact *VRoster::createContact(vk::Buddy *buddy)
{
	VContact *contact  = new VContact(buddy, m_account);
	connect(contact, SIGNAL(destroyed(QObject*)), SLOT(onContactDestroyed(QObject*)));
	m_contactHash.insert(buddy->id(), contact);
	emit m_account->contactCreated(contact);
	return contact;
}

void VRoster::onAddFriend(vk::Buddy *buddy)
{
	if (!m_contactHash.value(buddy->id()))
		createContact(buddy);
}

void VRoster::onOnlineChanged(bool isOnline)
{
	if (isOnline) {
		vk::Reply *reply = m_account->client()->roster()->getMessages(0, 50, vk::Message::FilterUnread);
		connect(reply, SIGNAL(resultReady(QVariant)), SLOT(onMessagesRecieved(QVariant)));
	}
}

void VRoster::onMessageAdded(const vk::Message &msg)
{
	if (false) {
		int id = msg.isIncoming() ? msg.fromId() : msg.toId();
		VGroupChat *c = groupChat(id);
		if (c)
			c->handleMessage(msg);
	} else {
		int id = msg.isIncoming() ? msg.fromId() : msg.toId();
		VContact *c = contact(id);
		if (c)
			c->handleMessage(msg);
		else
			warning() << "Unable to find reciever with id in roster" << id;
	}
}

void VRoster::onContactDestroyed(QObject *obj)
{
	m_contactHash.remove(m_contactHash.key(static_cast<VContact*>(obj)));
}

void VRoster::onGroupChatDestroyed(QObject *obj)
{
	m_contactHash.remove(m_contactHash.key(static_cast<VContact*>(obj)));
}

void VRoster::onContactTyping(int userId, int chatId)
{
	if (!chatId) {
		VContact *c = contact(userId);
		c->setTyping(true);
	} else {
		VGroupChat *c = groupChat(chatId);
		//TODO
		//c->setChatState(ChatStateComposing);
	}
}

void VRoster::onMessagesRecieved(const QVariant &response)
{
	QVariantList list = response.toList();
	list.removeFirst();
	vk::MessageList msgList = vk::Message::fromVariantList(list, m_account->client());
	foreach (vk::Message msg, msgList)
		if (msg.isUnread() && msg.isIncoming())
			onMessageAdded(msg);
}

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

#include <vk/roster.h>
#include <vk/longpoll.h>
#include <vk/message.h>

#include <qutim/chatsession.h>

VRoster::VRoster(VAccount *account) : QObject(account),
	m_account(account)
{
	connect(m_account->client()->roster(), SIGNAL(friendAdded(vk::Buddy*)), SLOT(onAddFriend(vk::Buddy*)));

	vk::LongPoll *poll = m_account->client()->longPoll();
	connect(poll, SIGNAL(messageAdded(vk::Message)), SLOT(onMessageAdded(vk::Message)));
	connect(poll, SIGNAL(contactStatusChanged(int,vk::Buddy::Status)), SLOT(onContactStatusChanged(int,vk::Buddy::Status)));
	connect(poll, SIGNAL(contactTyping(int)), SLOT(onContactTyping(int)));
}

VContact *VRoster::contact(int id, bool create)
{
	VContact *c = m_contactHash.value(id);
	if (!c && create && id != m_account->uid()) {
		vk::Buddy *buddy = static_cast<vk::Buddy*>(m_account->client()->roster()->contact(id));
		c = createContact(buddy);
	}
	return c;
}

VContact *VRoster::contact(int id) const
{
	return m_contactHash.value(id);
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
	createContact(buddy);
}

void VRoster::onMessageAdded(const vk::Message &msg)
{
	if (msg.fromId() == m_account->uid()) {
		VContact *unit = contact(msg.toId());

	} else {
		VContact *unit = contact(msg.fromId());
		qutim_sdk_0_3::Message coreMessage(msg.body());
		coreMessage.setChatUnit(unit);
		coreMessage.setIncoming(true);
		qutim_sdk_0_3::ChatLayer::get(unit, true)->appendMessage(coreMessage);
	}

}

void VRoster::onContactDestroyed(QObject *obj)
{
	m_contactHash.remove(m_contactHash.key(static_cast<VContact*>(obj)));
}

void VRoster::onContactTyping(int userId)
{
	VContact *c = contact(userId);
	c->setTyping(true);
}


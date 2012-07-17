/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#include "vgroupchat.h"
#include <vk/groupchatsession.h>
#include <vcontact.h>

using namespace qutim_sdk_0_3;

VGroupChat::VGroupChat(VAccount *account, int chatId) :
	Conference(account),
	m_account(account),
	m_chatSession(new vk::GroupChatSession(chatId, account->client()))
{
	m_chatSession->setParent(this);
	m_title = m_chatSession->title();
	if (m_chatSession->client()->isOnline()) {
		setJoined(true);
		m_chatSession->getInfo();
		m_chatSession->getHistory();
	}
	connect(m_chatSession, SIGNAL(participantAdded(vk::Buddy*)), this, SLOT(onBuddyAdded(vk::Buddy*)));
	connect(m_chatSession, SIGNAL(participantRemoved(vk::Buddy*)), this, SLOT(onBuddyRemoved(vk::Buddy*)));
	connect(m_chatSession, SIGNAL(titleChanged(QString)), SLOT(onTitleChanged(QString)));
	connect(account->client(), SIGNAL(onlineStateChanged(bool)), SLOT(setJoined(bool)));
}

VGroupChat::~VGroupChat()
{
}

void VGroupChat::setTyping(int uid, bool set)
{
	VContact *c = findContact(uid);
	if (c)
		c->setTyping(set);
}

VContact *VGroupChat::findContact(int uid) const
{
	return m_buddies.value(qobject_cast<vk::Buddy*>(m_chatSession->findParticipant(uid)));
}

Buddy *VGroupChat::me() const
{
	return m_account->me();
}

void VGroupChat::doJoin()
{
	m_chatSession->addParticipant(m_account->client()->me());
}

void VGroupChat::doLeave()
{
	m_chatSession->removeParticipant(m_account->client()->me());
}

void VGroupChat::onBuddyAdded(vk::Buddy *buddy)
{
	VContact *user = new VContact(buddy, m_account);
	if (ChatSession *session = ChatLayer::get(this, false))
		session->addContact(user);
	m_buddies.insert(buddy, user);
	connect(user, SIGNAL(destroyed(QObject*)), SLOT(onUserDestroyed(QObject*)));
}

void VGroupChat::onBuddyRemoved(vk::Buddy *buddy)
{
	VContact *user = m_buddies.take(buddy);
	if (ChatSession *session = ChatLayer::get(this, false))
		session->removeContact(user);

	if (ChatSession *session = ChatLayer::get(user, false)) {
		QObject::connect(session, SIGNAL(destroyed()), user, SLOT(deleteLater()));
	} else {
		m_buddies.remove(buddy);
		user->deleteLater();
	}
}

void VGroupChat::onUserDestroyed(QObject *obj)
{
	VContact *user = static_cast<VContact*>(obj);
	m_buddies.remove(m_buddies.key(user));
}

void VGroupChat::onJoinedChanged(bool set)
{
	if (set) {
		m_chatSession->getHistory();
		m_chatSession->getInfo();
	}
}

QString VGroupChat::id() const
{
	return QString::number(m_chatSession->uid());
}

QString VGroupChat::title() const
{
	return m_chatSession->title();
}

ChatUnitList VGroupChat::lowerUnits()
{
	ChatUnitList list;
	foreach (ChatUnit *unit, m_buddies.values())
		list.append(unit);
	return list;
}

bool VGroupChat::sendMessage(const Message &message)
{
	if (!m_account->client()->isOnline())
		return false;
	m_chatSession->sendMessage(message.text(), message.property("subject").toString());
	return true;
}

void VGroupChat::onTitleChanged(const QString &title)
{
	if (m_title != title) {
		QString old = m_title;
		m_title = title;
		emit titleChanged(title, old);
	}
}

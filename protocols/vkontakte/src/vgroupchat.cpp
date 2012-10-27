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
#include <vreen/groupchatsession.h>
#include "vroster.h"
#include <vcontact.h>
#include <QApplication>
#include <qutim/notification.h>

using namespace qutim_sdk_0_3;

VGroupChat::VGroupChat(VAccount *account, int chatId) :
	Conference(account),
	m_account(account),
	m_chatSession(new Vreen::GroupChatSession(chatId, account->client())),
	m_unreachedMessagesCount(0)
{
	m_chatSession->setParent(this);
	m_title = m_chatSession->title();

	connect(m_chatSession, SIGNAL(participantAdded(Vreen::Buddy*)), this, SLOT(onBuddyAdded(Vreen::Buddy*)));
	connect(m_chatSession, SIGNAL(participantRemoved(Vreen::Buddy*)), this, SLOT(onBuddyRemoved(Vreen::Buddy*)));
	connect(m_chatSession, SIGNAL(titleChanged(QString)), SLOT(onTitleChanged(QString)));
	connect(m_chatSession, SIGNAL(messageAdded(Vreen::Message)), SLOT(handleMessage(Vreen::Message)));
	connect(ChatLayer::instance(), SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)),
			SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*)));

	connect(m_chatSession, SIGNAL(isJoinedChanged(bool)), SLOT(setJoined(bool)));
	connect(this, SIGNAL(joinedChanged(bool)), SLOT(onJoinedChanged(bool)));
	setJoined(m_chatSession->client()->isOnline());
}

VGroupChat::~VGroupChat()
{
}

void VGroupChat::setTyping(int uid, bool set)
{
	VContact *c = contact(uid);
	if (c)
		c->setTyping(set);
}

VContact *VGroupChat::findContact(int uid) const
{
	return m_buddies.value(qobject_cast<Vreen::Buddy*>(m_chatSession->findParticipant(uid)));
}

VContact *VGroupChat::contact(int uid)
{
	return m_account->contact(uid);
}

void VGroupChat::handleMessage(const Vreen::Message &msg)
{
	if (!msg.fromId()) {
		Vreen::Reply *reply = m_account->client()->getMessage(msg.id());
		connect(reply, SIGNAL(resultReady(QVariant)), SLOT(onMessageGet(QVariant)));
	} else {
		if (!msg.isIncoming() && m_unreachedMessagesCount) {
			m_pendingMessages.append(msg);
			return;
		}

		SentMessagesList::iterator i = m_sentMessages.begin();
		for (; i != m_sentMessages.end(); ++i) {
			if (i->second == msg.id()) {
				ChatSession *s = ChatLayer::get(this);
				qApp->postEvent(s, new MessageReceiptEvent(i->first, true));
				m_sentMessages.removeAt(i - m_sentMessages.begin());
				return;
			}
		}

		qutim_sdk_0_3::Message coreMessage(msg.body().replace("<br>", "\n"));
		coreMessage.setChatUnit(this);
		coreMessage.setIncoming(msg.isIncoming());

		coreMessage.setProperty("mid", msg.id());
		coreMessage.setProperty("subject", msg.subject());

		VContact *from = contact(msg.fromId());
		coreMessage.setProperty("senderName", from->name());
		coreMessage.setProperty("senderId", from->id());

		ChatSession *s = ChatLayer::get(this);
		if (msg.isIncoming()) {
			if (!s->isActive())
				m_unreadMessages.append(coreMessage);
			else
				chatSession()->markMessagesAsRead(Vreen::IdList() << msg.id(), true);
		} else
			coreMessage.setProperty("history", true);
		s->appendMessage(coreMessage);
	}
}

Buddy *VGroupChat::me() const
{
	return m_account->me();
}

void VGroupChat::doJoin()
{
	m_chatSession->join();
}

void VGroupChat::doLeave()
{
	m_chatSession->leave();
}

void VGroupChat::onBuddyAdded(Vreen::Buddy *buddy)
{
	if (!m_buddies.contains(buddy)) {
		VContact *user = new VContact(buddy, m_account);
		if (ChatSession *session = ChatLayer::get(this, false)) {
			session->addContact(user);

			NotificationRequest request(qutim_sdk_0_3::Notification::ChatUserJoined);
			request.setObject(this);
			request.setText(tr("%1 has joined the room").arg(user->title()));
			request.setProperty("senderName", user->name());
			request.send();
		}
		m_buddies.insert(buddy, user);
		connect(user, SIGNAL(destroyed(QObject*)), SLOT(onUserDestroyed(QObject*)));
	}
}

void VGroupChat::onBuddyRemoved(Vreen::Buddy *buddy)
{
	VContact *user = m_buddies.take(buddy);
	if (user) {
		if (ChatSession *session = ChatLayer::get(this, false)) {
			session->removeContact(user);

			NotificationRequest request(qutim_sdk_0_3::Notification::ChatUserLeft);
			request.setObject(this);
			request.setText(tr("%1 has left the room").arg(user->title()));
			request.setProperty("senderName", user->name());
			request.send();
		}

		if (!user->isInList()) {
			if (ChatSession *session = ChatLayer::get(user, false)) {
				QObject::connect(session, SIGNAL(destroyed()), user, SLOT(deleteLater()));
			} else {
				m_buddies.remove(buddy);
				user->deleteLater();
			}
		}
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
		//m_chatSession->getHistory();
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

ChatUnit *VGroupChat::findParticipant(int uid) const
{
	Vreen::Buddy *buddy = m_chatSession->findParticipant(uid);
	return m_buddies.value(buddy);
}

Vreen::GroupChatSession *VGroupChat::chatSession() const
{
	return m_chatSession;
}

bool VGroupChat::sendMessage(const Message &message)
{
	if (!m_account->client()->isOnline())
		return false;
	Vreen::Reply *reply = m_chatSession->sendMessage(message.text(), message.property("subject").toString());
	reply->setProperty("id", message.id());
	connect(reply, SIGNAL(resultReady(QVariant)), SLOT(onMessageSent(QVariant)));
	m_unreachedMessagesCount++;
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

void VGroupChat::onMessageGet(const QVariant &response)
{
	QVariantList list = response.toList();
	if (list.count()) {
		Q_UNUSED(list.takeFirst());
		Vreen::MessageList messageList = Vreen::Message::fromVariantList(list, m_account->client());
		foreach (Vreen::Message msg, messageList)
			handleMessage(msg);
	}
}

void VGroupChat::onMessageSent(const QVariant &response)
{
	m_unreachedMessagesCount--;
	int mid = response.toInt();
	if (mid) {
		int id = sender()->property("id").toInt();
		m_sentMessages << QPair<int, int>(id, mid);
	} else {
		//TODO undelivered messages check
	}

	if (!m_unreachedMessagesCount) {
		foreach (Vreen::Message msg, m_pendingMessages) {
			handleMessage(msg);
		}
		m_pendingMessages.clear();
	}
}

void VGroupChat::onUnreadChanged(qutim_sdk_0_3::MessageList unread)
{
	Vreen::IdList idList;
	MessageList::iterator i = m_unreadMessages.begin();
	for (; i != m_unreadMessages.end(); ++i) {
		int index = -1;
		MessageList::iterator j = unread.begin();
		for (; j != unread.end(); ++j) {
            if (i->property("mid") == j->property("mid")) {
				index = j - unread.begin();
				unread.removeAt(index);
				break;
			}
		}
        if (index == -1)
            idList.append(m_unreadMessages.takeAt(i-m_unreadMessages.begin()).property("mid").toInt());
	}
	if (idList.count())
		chatSession()->markMessagesAsRead(idList, true);
}

void VGroupChat::onSessionCreated(qutim_sdk_0_3::ChatSession *session)
{
	if (session->unit() == this)
		connect(session, SIGNAL(unreadChanged(qutim_sdk_0_3::MessageList)), SLOT(onUnreadChanged(qutim_sdk_0_3::MessageList)));
}

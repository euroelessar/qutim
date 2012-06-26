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

#include "vcontact.h"
#include "vaccount.h"
#include "vroster.h"
#include "vinforequest.h"

#include <qutim/tooltip.h>
#include <qutim/inforequest.h>
#include <qutim/notification.h>
#include <qutim/message.h>
#include <qutim/chatsession.h>

#include <vk/contact.h>
#include <vk/chatsession.h>

#include <QTimer>
#include <QApplication>

using namespace qutim_sdk_0_3;

VContact::VContact(vk::Buddy *contact, VAccount* account): Contact(account),
	m_buddy(contact)
{
	m_status = Status::instance(m_buddy->isOnline() ? Status::Online : Status::Offline, "vkontakte");
	m_status.setText(m_buddy->activity());
	m_name = m_buddy->name();
	m_tags = m_buddy->tags();

	connect(m_buddy, SIGNAL(statusChanged(vk::Contact::Status)), SLOT(onStatusChanged(vk::Contact::Status)));
	connect(m_buddy, SIGNAL(activityChanged(QString)), SLOT(onActivityChanged(QString)));
	connect(m_buddy, SIGNAL(nameChanged(QString)), SLOT(onNameChanged(QString)));
	connect(m_buddy, SIGNAL(tagsChanged(QStringList)), SLOT(onTagsChanged(QStringList)));
	connect(ChatLayer::instance(), SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)), SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*)));
}


QString VContact::id() const
{
	return QString::number(m_buddy->id());
}

bool VContact::isInList() const
{
	return m_buddy->isFriend();
}

bool VContact::sendMessage(const Message& message)
{
	if (!m_buddy->client()->isOnline())
		return false;
	vk::Reply *reply = chatSession()->sendMessage(message.text(),
												  message.property("subject").toString()); //TODO don't use vk::Reply, use vlongpoll instead
	reply->setProperty("id", message.id());
	connect(reply, SIGNAL(resultReady(QVariant)), SLOT(onMessageSent(QVariant)));
	return true;
}

void VContact::setTags(const QStringList& tags)
{
	Q_UNUSED(tags);
}

void VContact::setInList(bool inList)
{
	Q_UNUSED(inList);
}

Status VContact::status() const
{
	return m_status;
}

QString VContact::activity() const
{
	return m_status.text();
}

void VContact::handleMessage(const vk::Message &msg)
{
	SentMessagesList::iterator i = m_sentMessages.begin();
	for (; i != m_sentMessages.end(); i++) {
		if (i->second == msg.id()) {
			ChatSession *s = ChatLayer::get(this);
			qApp->postEvent(s, new MessageReceiptEvent(i->first, true));
			m_sentMessages.removeAt(i - m_sentMessages.begin());
			return;
			return;
		}
	}
	qutim_sdk_0_3::Message coreMessage(msg.body().replace("<br>", "\n"));
	coreMessage.setChatUnit(this);
	coreMessage.setIncoming(msg.isIncoming());
	coreMessage.setProperty("mid", msg.id());
	coreMessage.setProperty("subject", msg.subject());

	qutim_sdk_0_3::ChatSession *s = ChatLayer::get(this);
	if (msg.isIncoming()) {
		if (!s->isActive())
			m_unreadMessages.append(coreMessage);
		else
			m_chatSession->markMessagesAsRead(vk::IdList() << msg.id(), true);
	} else
		coreMessage.setProperty("history", true);
	s->appendMessage(coreMessage);
}

void VContact::setStatus(const Status &status)
{
	Status old = m_status;
	m_status = status;
	emit statusChanged(status, old);
	NotificationRequest request(this, status, old);
	request.send();
}

vk::ChatSession *VContact::chatSession()
{
	if (m_chatSession.isNull()) {
		m_chatSession = new vk::ChatSession(m_buddy);
		qutim_sdk_0_3::ChatSession *s = ChatLayer::get(this);
		m_chatSession->setParent(s);
	}
	return m_chatSession.data();
}

void VContact::setTyping(bool set)
{
	if (set) {
		if (m_typingTimer.isNull()) {
			m_typingTimer = new QTimer(this);
			m_typingTimer->setInterval(5000);
			connect(m_typingTimer, SIGNAL(timeout()), SLOT(setTyping()));
			connect(m_typingTimer, SIGNAL(timeout()), m_typingTimer, SLOT(deleteLater()));
		}
		m_typingTimer->start();
		setChatState(ChatStateComposing);
	} else
		setChatState(ChatStateActive);
}

void VContact::onActivityChanged(const QString &activity)
{
	m_status.setText(activity);
	setStatus(m_status);
}

VContact::~VContact()
{

}

QStringList VContact::tags() const
{
	return m_tags;
}

QString VContact::name() const
{
	return m_name;
}

void VContact::setName(const QString& name)
{
	Q_UNUSED(name);
}

QString VContact::avatar() const
{
	return m_buddy->photoSource();
}

bool VContact::event(QEvent *ev)
{
	if (ev->type() == ToolTipEvent::eventType()) {
		ToolTipEvent *event = static_cast<ToolTipEvent*>(ev);
		if (!activity().isEmpty())
			event->addField(QT_TRANSLATE_NOOP("ContactInfo", "Activity"),
							activity());
	}
	return Contact::event(ev);
}


void VContact::onStatusChanged(vk::Contact::Status status)
{
	Status::Type type;
	switch (status) {
	case vk::Contact::Offline:
		type = Status::Offline;
		break;
	case vk::Contact::Online:
		type = Status::Online;
		break;
	case vk::Contact::Away:
		type = Status::Away;
	default:
		break;
	}
	m_status.setType(type);
	setStatus(m_status);
}

void VContact::onTagsChanged(const QStringList &tags)
{
	QStringList old = m_tags;
	m_tags = tags;
	emit tagsChanged(tags, old);
}

void VContact::onNameChanged(const QString &name)
{
	QString old = m_name;
	m_name = name;
	emit nameChanged(name, old);
}

void VContact::onMessageSent(const QVariant &response)
{
	int mid = response.toInt();
	if (mid) {
		int id = sender()->property("id").toInt();
		m_sentMessages << QPair<int, int>(id, mid);
	}
}

void VContact::onUnreadChanged(MessageList unread)
{
	vk::IdList idList;
	MessageList::iterator i = m_unreadMessages.begin();
	for (; i != m_unreadMessages.end(); i++) {
		int index = -1;
		MessageList::iterator j = unread.begin();
		for (; j != unread.end(); j++) {
			if (i->id() == j->id()) {
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

	idList.clear();
	//foreach (Message msg, unread)
	//	idList.append(msg.property("mid").toInt());
	//chatSession()->markMessagesAsRead(idList, false);
}

void VContact::onSessionCreated(ChatSession *session)
{
	if (session->unit() == this)
		connect(session, SIGNAL(unreadChanged(qutim_sdk_0_3::MessageList)), SLOT(onUnreadChanged(qutim_sdk_0_3::MessageList)));
}

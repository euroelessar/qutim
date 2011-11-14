/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "chatsessionadapter.h"
#include "chatunitadaptor.h"

Q_GLOBAL_STATIC(ChatSessionPathHash, chatSessionHash)
Q_GLOBAL_STATIC_WITH_ARGS(int, counter, (0))

const ChatSessionPathHash &ChatSessionAdapter::hash()
{
	return *chatSessionHash();
}

QDBusObjectPath ChatSessionAdapter::ensurePath(QDBusConnection dbus, ChatSession *session)
{
	QDBusObjectPath path = chatSessionHash()->value(session);
	if (path.path().isEmpty() && session) {
		ChatSessionAdapter *adapter = new ChatSessionAdapter(dbus, session);
		path = adapter->path();
		dbus.registerObject(path.path(), session, QDBusConnection::ExportAdaptors);
	}
	return path.path().isEmpty() ? QDBusObjectPath("/") : path;
}

ChatSessionAdapter::ChatSessionAdapter(const QDBusConnection &dbus, ChatSession *session) :
		QDBusAbstractAdaptor(session), m_session(session), m_dbus(dbus)
{
	m_path = QDBusObjectPath(QLatin1String("/ChatSession/") + QString::number(*counter()));
	(*counter())++;
	chatSessionHash()->insert(session, m_path);
	connect(session, SIGNAL(activated(bool)), this, SIGNAL(activated(bool)));
	connect(session, SIGNAL(unreadChanged(qutim_sdk_0_3::MessageList)),
			this, SIGNAL(unreadChanged(qutim_sdk_0_3::MessageList)));
	connect(session, SIGNAL(contactAdded(qutim_sdk_0_3::Buddy*)),
			this, SLOT(onContactAdded(qutim_sdk_0_3::Buddy*)));
	connect(session, SIGNAL(contactRemoved(qutim_sdk_0_3::Buddy*)),
			this, SLOT(onContactRemoved(qutim_sdk_0_3::Buddy*)));
	connect(session, SIGNAL(messageReceived(qutim_sdk_0_3::Message*)),
			this, SLOT(onMessageReceived(qutim_sdk_0_3::Message*)));
	connect(session, SIGNAL(messageSent(qutim_sdk_0_3::Message*)),
			this, SLOT(onMessageSent(qutim_sdk_0_3::Message*)));
}

ChatSessionAdapter::~ChatSessionAdapter()
{
	chatSessionHash()->remove(m_session);
}

QDBusObjectPath ChatSessionAdapter::chatUnit()
{
	return ChatUnitAdaptor::ensurePath(m_dbus, m_session->getUnit());
}

void ChatSessionAdapter::setChatUnit(const QDBusObjectPath &unitPath)
{
	QObject *obj = m_dbus.objectRegisteredAt(unitPath.path());
	ChatUnit *unit = qobject_cast<ChatUnit*>(obj);
	if (unit)
		m_session->setChatUnit(unit);
}

void ChatSessionAdapter::addContact(const QDBusObjectPath &buddyPath)
{
	QObject *obj = m_dbus.objectRegisteredAt(buddyPath.path());
	Buddy *buddy = qobject_cast<Buddy*>(obj);
	if (buddy)
		m_session->addContact(buddy);
}

void ChatSessionAdapter::removeContact(const QDBusObjectPath &buddyPath)
{
	QObject *obj = m_dbus.objectRegisteredAt(buddyPath.path());
	Buddy *buddy = qobject_cast<Buddy*>(obj);
	if (buddy)
		m_session->removeContact(buddy);
}

void ChatSessionAdapter::onMessageReceived(qutim_sdk_0_3::Message *message)
{
	emit messageReceived(*message);
}

void ChatSessionAdapter::onMessageSent(qutim_sdk_0_3::Message *message)
{
	emit messageSent(*message);
}

void ChatSessionAdapter::onContactAdded(qutim_sdk_0_3::Buddy *buddy)
{
	QDBusObjectPath path = ChatUnitAdaptor::ensurePath(m_dbus, buddy);
	emit contactAdded(path, buddy->id());
}

void ChatSessionAdapter::onContactRemoved(qutim_sdk_0_3::Buddy *buddy)
{
	QDBusObjectPath path = ChatUnitAdaptor::ensurePath(m_dbus, buddy);
	emit contactRemoved(path, buddy->id());
}


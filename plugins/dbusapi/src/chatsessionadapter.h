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

#ifndef CHATSESSIONADAPTER_H
#define CHATSESSIONADAPTER_H

#include <qutim/chatsession.h>
#include <QDBusAbstractAdaptor>
#include <QDBusObjectPath>
#include <QDBusConnection>

using namespace qutim_sdk_0_3;

typedef QMap<ChatSession*, QDBusObjectPath> ChatSessionPathHash;

class ChatSessionAdapter : public QDBusAbstractAdaptor
{
	Q_OBJECT
	Q_CLASSINFO("D-Bus Interface", "org.qutim.ChatSession")
	Q_PROPERTY(QDBusObjectPath chatUnit READ chatUnit WRITE setChatUnit)
	Q_PROPERTY(bool active READ isActive WRITE setActive NOTIFY activated)
	Q_PROPERTY(qutim_sdk_0_3::MessageList unread READ unread NOTIFY unreadChanged)
public:
	static const ChatSessionPathHash &hash();
	static QDBusObjectPath ensurePath(QDBusConnection dbus, ChatSession *session);
	
    ChatSessionAdapter(const QDBusConnection &dbus, ChatSession *session);
	virtual ~ChatSessionAdapter();
	
	QDBusObjectPath chatUnit();
	void setChatUnit(const QDBusObjectPath &unit);
	inline bool isActive() { return m_session->isActive(); }
	inline void setActive(bool active) { m_session->setActive(active); }
	inline MessageList unread() const { return m_session->unread(); }
	inline const QDBusObjectPath &path() const { return m_path; }
	
public slots:
	void addContact(const QDBusObjectPath &buddy);
	void removeContact(const QDBusObjectPath &buddy);
	inline qint64 appendMessage(qutim_sdk_0_3::Message &message);
	inline qint64 appendMessage(const QString &text);
	inline void activate() { setActive(true); }
	inline void markRead(quint64 id) { m_session->markRead(id); }
	
signals:
	void messageReceived(const qutim_sdk_0_3::Message &message);
	void messageSent(const qutim_sdk_0_3::Message &message);
	void contactAdded(const QDBusObjectPath &buddy, const QString &id);
	void contactRemoved(const QDBusObjectPath &buddy, const QString &id);
	void activated(bool active);
	void unreadChanged(const qutim_sdk_0_3::MessageList &messages);
	
private slots:
	void onMessageReceived(qutim_sdk_0_3::Message *message);
	void onMessageSent(qutim_sdk_0_3::Message *message);
	void onContactAdded(qutim_sdk_0_3::Buddy *c);
	void onContactRemoved(qutim_sdk_0_3::Buddy *c);
	
private:
	ChatSession *m_session;
	QDBusConnection m_dbus;
	QDBusObjectPath m_path;
};

qint64 ChatSessionAdapter::appendMessage(qutim_sdk_0_3::Message &message)
{
	return m_session->appendMessage(message);
}

qint64 ChatSessionAdapter::appendMessage(const QString &text)
{
	return m_session->appendMessage(text);
}

#endif // CHATSESSIONADAPTER_H


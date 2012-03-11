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

#ifndef LIBQUTIM_CHATSESSION_H
#define LIBQUTIM_CHATSESSION_H

#include "message.h"
#include "contact.h"
#include <QDateTime>

class QTextDocument;
namespace qutim_sdk_0_3
{

class Account;
class ChatLayer;
class ChatSessionPrivate;
class ChatLayerPrivate;

class LIBQUTIM_EXPORT ChatSession : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(ChatSession)
	Q_PROPERTY(bool active READ isActive WRITE setActive NOTIFY activated)
	Q_PROPERTY(qutim_sdk_0_3::MessageList unread READ unread NOTIFY unreadChanged)
	Q_PROPERTY(QDateTime dateOpened READ dateOpened WRITE setDateOpened NOTIFY dateOpenedChanged)
public:
	virtual ~ChatSession();
	
	virtual ChatUnit *getUnit() const = 0;
	Q_INVOKABLE inline qutim_sdk_0_3::ChatUnit *unit() const { return getUnit(); }
	Q_INVOKABLE virtual void setChatUnit(qutim_sdk_0_3::ChatUnit* unit) = 0;
	Q_INVOKABLE qint64 append(qutim_sdk_0_3::Message &message);
	virtual QTextDocument *getInputField() = 0;
	virtual void markRead(quint64 id) = 0;
	virtual MessageList unread() const = 0;
	bool isActive();
	QDateTime dateOpened() const;
	void setDateOpened(const QDateTime &date);
public slots:
	virtual void addContact(qutim_sdk_0_3::Buddy *c) = 0;
	virtual void removeContact(qutim_sdk_0_3::Buddy *c) = 0;
	qint64 appendMessage(qutim_sdk_0_3::Message &message);
	void setActive(bool active);
	inline void activate() { setActive(true); }
	inline qint64 appendMessage(const QString &text)
	{ Message msg(text); return appendMessage(msg); }
protected:
	virtual void doSetActive(bool active) = 0;
	virtual qint64 doAppendMessage(qutim_sdk_0_3::Message &message) = 0;
signals:
	void dateOpenedChanged(const QDateTime &date);
	void messageReceived(qutim_sdk_0_3::Message *message);
	void messageSent(qutim_sdk_0_3::Message *message);
	void contactAdded(qutim_sdk_0_3::Buddy *c);
	void contactRemoved(qutim_sdk_0_3::Buddy *c);
	void activated(bool active);
	void unreadChanged(const qutim_sdk_0_3::MessageList &);
protected:
	ChatSession(ChatLayer *chat);
	virtual void virtual_hook(int id, void *data);
	friend class MessageHandlerHook;
private:
	QScopedPointer<ChatSessionPrivate> d_ptr;
};

class LIBQUTIM_EXPORT ChatLayer : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(ChatLayer)
	Q_CLASSINFO("Service", "ChatLayer")
	Q_PROPERTY(bool alertStatus READ isAlerted WRITE alert NOTIFY alertStatusChanged)
public:
	static ChatLayer *instance();

	ChatSession *getSession(Account *acc, QObject *obj, bool create = true);
	ChatSession *getSession(QObject *obj, bool create = true);
	ChatSession *getSession(Account *acc, const QString &id, bool create = true);
	virtual ChatSession *getSession(ChatUnit *unit, bool create = true) = 0;
	Q_INVOKABLE inline qutim_sdk_0_3::ChatSession *session(QObject *obj, bool create = true);
	static ChatSession *get(ChatUnit *unit, bool create = true);
	Q_INVOKABLE virtual QList<qutim_sdk_0_3::ChatSession*> sessions() = 0;
	bool isAlerted() const;
	void alert(bool on);
	void alert(int msecs);
	bool event(QEvent *);
	
signals:
	void sessionCreated(qutim_sdk_0_3::ChatSession *session);
	void alertStatusChanged(bool);
protected:
	ChatUnit *getUnitForSession(ChatUnit *unit) const;
	ChatLayer();
	virtual ~ChatLayer();
	virtual void virtual_hook(int id, void *data);
	QScopedPointer<ChatLayerPrivate> d_ptr;
};

ChatSession *ChatLayer::session(QObject *obj, bool create)
{
	return getSession(obj, create);
}

}

Q_DECLARE_METATYPE(qutim_sdk_0_3::ChatSession*)
Q_DECLARE_METATYPE(QList<qutim_sdk_0_3::ChatSession*>)

#endif // LIBQUTIM_CHATSESSION_H


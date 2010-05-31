/****************************************************************************
 *  messagesession.h
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#ifndef LIBQUTIM_MESSAGESESSION_H
#define LIBQUTIM_MESSAGESESSION_H

#include "message.h"
#include "contact.h"

class QTextDocument;
namespace qutim_sdk_0_3
{
	class Account;
	class ChatLayer;
	struct ChatSessionPrivate;

	class LIBQUTIM_EXPORT ChatSession : public QObject
	{
		Q_OBJECT
		Q_PROPERTY(bool active READ isActive WRITE setActive NOTIFY activated)
		Q_PROPERTY(qutim_sdk_0_3::MessageList unread READ unread NOTIFY unreadChanged)
	public:
		virtual ChatUnit *getUnit() const = 0;
		virtual void setChatUnit(qutim_sdk_0_3::ChatUnit* unit) = 0;
		virtual QTextDocument *getInputField() = 0;
		virtual void markRead(quint64 id) = 0;
		virtual MessageList unread() const = 0;
	public slots:
		virtual void addContact(qutim_sdk_0_3::Buddy *c) = 0;
		virtual void removeContact(qutim_sdk_0_3::Buddy *c) = 0;
		virtual qint64 appendMessage(qutim_sdk_0_3::Message &message) = 0;
		virtual bool isActive() = 0;
		virtual void setActive(bool active) = 0;
		inline void activate() { setActive(true); }
		inline qint64 appendMessage(const QString &text)
		{ Message msg(text); return appendMessage(msg); }
	signals:
		void messageReceived(qutim_sdk_0_3::Message *message);
		void messageSent(qutim_sdk_0_3::Message *message);
		void contactAdded(qutim_sdk_0_3::Buddy *c);
		void contactRemoved(qutim_sdk_0_3::Buddy *c);
		void activated(bool active);
		void unreadChanged(const qutim_sdk_0_3::MessageList &);
	protected:
		ChatSession(ChatLayer *chat);
		virtual ~ChatSession();
		virtual void virtual_hook(int id, void *data);
	private:
		QScopedPointer<ChatSessionPrivate> p;
	};

	class LIBQUTIM_EXPORT ChatLayer : public QObject
	{
		Q_OBJECT
		Q_CLASSINFO("Service", "ChatLayer")
	public:
		static ChatLayer *instance();

		ChatSession *getSession(Account *acc, QObject *obj, bool create = true);
		ChatSession *getSession(QObject *obj, bool create = true);
		ChatSession *getSession(Account *acc, const QString &id, bool create = true);
		virtual ChatSession *getSession(ChatUnit *unit, bool create = true) = 0;
		static ChatSession *get(ChatUnit *unit, bool create = true);
		virtual QList<ChatSession*> sessions() = 0;
	signals:
		void sessionCreated(qutim_sdk_0_3::ChatSession *session);
	protected:
		ChatUnit *getUnitForSession(ChatUnit *unit) const;
		ChatLayer();
		virtual ~ChatLayer();
		virtual void virtual_hook(int id, void *data);
	};
}

#endif // LIBQUTIM_MESSAGESESSION_H

/****************************************************************************
 *  messagesession.h
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

namespace qutim_sdk_0_3
{
	class Account;
	class ChatLayer;
	struct ChatSessionPrivate;

	class LIBQUTIM_EXPORT ChatSession : public QObject
	{
		Q_OBJECT
	public slots:
		virtual void addContact(Contact *c) = 0;
		virtual void removeContact(Contact *c) = 0;
		virtual void chatStateChanged(Contact *c, ChatState state) = 0;
		virtual void appendMessage(const Message &message) = 0;
		inline void appendMessage(const QString &text)
		{ appendMessage(Message(text)); }
	signals:
		void myselfChatStateChanged();
		void remoteChatStateChanged(Contact *c, ChatState state);
		void messageReceived(const Message &message);
		void messageSended(const Message &message);
		void contactAdded();
	protected:
                ChatSession(ChatLayer *chat);
		virtual ~ChatSession();
	private:
		QScopedPointer<ChatSessionPrivate> p;
	};

	class LIBQUTIM_EXPORT ChatLayer : public QObject
	{
		Q_OBJECT
	public:
		static ChatLayer *instance();
		inline ChatSession *getSession(Contact *c, bool create = true)
		{ return getSession(c->account(), c->id(), create); }
		inline ChatSession *getSession(Account *acc, QObject *obj, bool create = true)
		{ return getSession(acc, obj->property("id").toString(), create); }
		ChatSession *getSession(QObject *obj, bool create = true);
		virtual ChatSession *getSession(Account *acc, const QString &id, bool create = true) = 0;
		virtual QList<ChatSession*> sessions() = 0;
	signals:
		void sessionCreated(ChatSession *session);
	protected:
		ChatLayer();
		virtual ~ChatLayer();
	};
}

#endif // LIBQUTIM_MESSAGESESSION_H

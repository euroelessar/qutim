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
	public:
		virtual ChatUnit *getUnit() const = 0;
		virtual void setChatUnit(qutim_sdk_0_3::ChatUnit* unit) = 0;
		virtual QTextDocument *getInputField() = 0;
	public slots:
		virtual void addContact(qutim_sdk_0_3::ChatUnit *c) = 0;
		virtual void removeContact(qutim_sdk_0_3::ChatUnit *c) = 0;
		virtual void appendMessage(const qutim_sdk_0_3::Message &message) = 0;
		virtual bool isActive() = 0;
		virtual void setActive(bool active) = 0;
		inline void activate() { setActive(true); }
		inline void appendMessage(const QString &text)
		{ appendMessage(Message(text)); }
	signals:
		void messageReceived(const Message &message);
		void messageSended(const Message &message);
		void contactAdded();
		void activated(bool active);
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

		ChatSession *getSession(Account *acc, QObject *obj, bool create = true);
		ChatSession *getSession(QObject *obj, bool create = true);
		ChatSession *getSession(Account *acc, const QString &id, bool create = true);
		virtual ChatSession *getSession(ChatUnit *unit, bool create = true) = 0;
		static ChatSession *get(ChatUnit *unit, bool create = true);
		virtual QList<ChatSession*> sessions() = 0;
	signals:
		void sessionCreated(ChatSession *session);
	protected:
		ChatUnit *getUnitForSession(ChatUnit *unit) const;
		ChatLayer();
		virtual ~ChatLayer();
	};
}

#endif // LIBQUTIM_MESSAGESESSION_H

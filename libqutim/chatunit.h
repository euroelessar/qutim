/****************************************************************************
 *  chatunit.h
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

#ifndef CHATUNIT_H
#define CHATUNIT_H

#include "menucontroller.h"
#include <QMetaType>
#include <QEvent>

namespace qutim_sdk_0_3
{
	class Account;
	class Message;
	class ChatUnit;
	class ChatUnitPrivate;
	class Contact;
	typedef QList<ChatUnit *> ChatUnitList;

	class LIBQUTIM_EXPORT ChatUnit : public MenuController
	{
		Q_DECLARE_PRIVATE(ChatUnit)
		Q_OBJECT
		Q_PROPERTY(QString id READ id)
		Q_PROPERTY(QString title READ title)
		Q_PROPERTY(Account* account READ account)
	public:
		ChatUnit(Account *account);
		ChatUnit(ChatUnitPrivate &d, Account *account);
		virtual ~ChatUnit();
		virtual QString id() const = 0;
		virtual QString title() const;
		Account *account();
		const Account *account() const;
		virtual void sendMessage(const Message &message) = 0;
		virtual ChatUnitList lowerUnits();
		virtual ChatUnit *upperUnit();
	public slots:
		virtual void setChatState(ChatState state) = 0;
	};
	
	class LIBQUTIM_EXPORT ChatStateChangedEvent : public QEvent
	{
	public:
		ChatStateChangedEvent(ChatUnit *unit, ChatState state);
		inline ChatUnit *chatUnit() const {return m_unit;};
		inline ChatState chatState() const {return m_state;};
		static QEvent::Type eventType();
	protected:
		ChatUnit *m_unit;
		ChatState m_state;
	};
}

Q_DECLARE_METATYPE(qutim_sdk_0_3::ChatUnit*)

#endif // CHATUNIT_H

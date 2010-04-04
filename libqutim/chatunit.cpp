/****************************************************************************
 *  chatunit.cpp
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

#include "chatunit_p.h"
#include "account.h"
#include "messagesession.h"
#include <QCoreApplication>
#include "message.h"
#include "notificationslayer.h"

namespace qutim_sdk_0_3
{
	ChatUnit::ChatUnit(Account *account) : MenuController(*new ChatUnitPrivate(this), account)
	{
		d_func()->account = account;
	}

	ChatUnit::ChatUnit(ChatUnitPrivate &d, Account *account) : MenuController(d, account)
	{
		d_func()->account = account;
	}

	ChatUnit::~ChatUnit()
	{
	}

	QString ChatUnit::title() const
	{
		QString title = property("name").toString();
		return title.isEmpty() ? id() : title;
	}

	Account *ChatUnit::account()
	{
		return d_func()->account;
	}

	const Account *ChatUnit::account() const
	{
		return d_func()->account;
	}

	ChatUnitList ChatUnit::lowerUnits()
	{
		return ChatUnitList();
	}

	ChatUnit *ChatUnit::upperUnit()
	{
		return 0;
	}

	void ChatUnit::sendMessage(const QString &text)
	{
		Message message(text);
		message.setIncoming(false);
		sendMessage(message);
	}

	void ChatUnit::setChatState(ChatState state)
	{
		ChatStateEvent event(state);
		if (ChatSession *s = ChatLayer::get(this,false)) {
			qApp->sendEvent(s,&event);
		}
		if (state == ChatStateComposing)
			Notifications::sendNotification(Notifications::Typing,this);
	}

	ChatUnit *ChatUnit::buddy()
	{
		ChatUnit *u = this;
		while (u) {
			if (qobject_cast<Buddy*>(u))
				return u;
			u = u->upperUnit();
		}
		return this;
	}

	const ChatUnit *ChatUnit::buddy() const
	{
		return const_cast<ChatUnit*>(this)->buddy();
	}
	
	ChatStateEvent::ChatStateEvent(ChatState state) :
			QEvent(eventType()), m_state(state)
	{

	}

	QEvent::Type ChatStateEvent::eventType()
	{
		static QEvent::Type type = QEvent::Type(QEvent::registerEventType(QEvent::User + 102));
		return type;
	}

}

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
#include <QSet>
#include "message.h"
#include "notificationslayer.h"
#include "metacontact.h"
#include "conference.h"

namespace qutim_sdk_0_3
{
#ifndef QT_NO_DEBUG
Q_GLOBAL_STATIC(QSet<ChatUnit*>, all_chat_units)
#endif

LIBQUTIM_EXPORT QSet<ChatUnit*> get_all_chat_units()
{
#ifndef QT_NO_DEBUG
	return *all_chat_units();
#else
	return QSet<ChatUnit*>();
#endif
}

ChatUnit::ChatUnit(Account *account) : MenuController(*new ChatUnitPrivate(this), account)
{
	d_func()->account = account;
#ifndef QT_NO_DEBUG
	all_chat_units()->insert(this);
#endif
}

ChatUnit::ChatUnit(ChatUnitPrivate &d, Account *account) : MenuController(d, account)
{
	d_func()->account = account;
#ifndef QT_NO_DEBUG
	all_chat_units()->insert(this);
#endif
}

ChatUnit::~ChatUnit()
{
#ifndef QT_NO_DEBUG
	all_chat_units()->remove(this);
#endif
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

quint64 ChatUnit::sendMessage(const QString &text)
{
	Message message(text);
	message.setIncoming(false);
	bool ok = sendMessage(message);
	return ok ? message.id() : -1;
}

void ChatUnit::setChatState(ChatState state)
{
	Q_D(ChatUnit);
	emit chatStateChanged(state,d->chatState);
	d->chatState = state;
	if (state == ChatStateComposing)
		Notifications::send(Notifications::Typing,this);
}

ChatState ChatUnit::chatState() const
{
	return d_func()->chatState;
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

ChatUnit* ChatUnit::metaContact()
{
	ChatUnit *u = this;
	while (u) {
		if (qobject_cast<MetaContact*>(u))
			return u;
		u = u->upperUnit();
	}
	return 0;
}

const ChatUnit* ChatUnit::metaContact() const
{
	return const_cast<ChatUnit*>(this)->metaContact();
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

const ChatUnit* ChatUnit::getHistoryUnit() const
{
	ChatUnit *u = const_cast<ChatUnit *>(this);
	ChatUnit *buddy = 0;
	if (qobject_cast<Contact*>(u))
		return u;
	else if (qobject_cast<Buddy*>(u))
		buddy = u;
	ChatUnit *p;
	while ((p = u->upperUnit()) != 0) {
		if (qobject_cast<MetaContact*>(p)) {
			break;
		} else if (qobject_cast<Contact*>(p)) {
			buddy = p;
			break;
		} else if (qobject_cast<Conference*>(p)) {
			break;
		} else if (qobject_cast<Buddy*>(p) && !buddy) {
			buddy = p;
		}
		u = p;
	}
	return buddy ? buddy : u;
}

}

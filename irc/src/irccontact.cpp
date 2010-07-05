/****************************************************************************
 *  irccontact.cpp
 *
 *  Copyright (c) 2010 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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


#include "irccontact_p.h"
#include "ircaccount_p.h"
#include <qutim/messagesession.h>
#include "QApplication"

namespace qutim_sdk_0_3 {

namespace irc {

void IrcContactPrivate::ref()
{
	m_ref.ref();
}

void IrcContactPrivate::deref()
{
	if (m_ref.deref()) {
		ChatSession *session = ChatLayer::instance()->getSession(q, false);
		if (!session)
			q->deleteLater();
	}
}

void IrcContactPrivate::updateNick(const QString &newNick)
{
	nick = newNick;
	emit q->nameChanged(nick);
}

IrcContact::IrcContact(IrcAccount *account, const QString &nick) :
	Buddy(account), d(new IrcContactPrivate)
{
	d->q = this;
	d->nick = nick;
}

IrcContact::~IrcContact()
{
}

QString IrcContact::id() const
{
	return d->nick;
}

bool IrcContact::sendMessage(const Message &message)
{
	Status::Type status = account()->status().type();
	if (status == Status::Connecting || status == Status::Offline)
		return false;
	account()->send(QString("PRIVMSG %1 :%2").arg(d->nick).arg(message.text()));
	return true;
}

const IrcAccount *IrcContact::account() const
{
	const Account *acc = Buddy::account();
	Q_ASSERT(qobject_cast<const IrcAccount*>(acc));
	return reinterpret_cast<const IrcAccount*>(acc);
}

IrcAccount *IrcContact::account()
{
	Account *acc = Buddy::account();
	Q_ASSERT(qobject_cast<IrcAccount*>(acc));
	return reinterpret_cast<IrcAccount*>(acc);
}

void IrcContact::onSessionDestroyed()
{
	if (d->m_ref == 0)
		deleteLater();
}

} } // namespace qutim_sdk_0_3::irc

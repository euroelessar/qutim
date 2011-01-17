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
#include "ircavatar.h"
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
	if (m_ref.deref() && ChatLayer::instance()) {
		ChatSession *session = ChatLayer::instance()->getSession(q, false);
		if (!session)
			q->deleteLater();
	}
}

void IrcContactPrivate::updateNick(const QString &newNick)
{
	QString previous = nick;
	nick = newNick;
	emit q->nameChanged(nick, previous);
}

IrcContact::IrcContact(IrcAccount *account, const QString &nick, const QString &host) :
	Buddy(account), d(new IrcContactPrivate)
{
	d->q = this;
	d->nick = nick;
	setHost(host);
	//IrcAvatar::instance()->requestAvatar(this);
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

QString IrcContact::avatar() const
{
	return d->avatar;
}

Status IrcContact::status() const
{
	Status status;
	status.setType(d->awayMsg.isEmpty() ? Status::Online : Status::Away);
	status.setText(d->awayMsg);
	return status;
}

void IrcContact::setAvatar(const QString &avatar)
{
	d->avatar = avatar;
	emit avatarChanged(avatar);
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

QSet<QChar> IrcContact::modes()
{
	return d->modes;
}

QString IrcContact::hostMask() const
{
	return d->hostMask;
}

QString IrcContact::domain() const
{
	return d->domain;
}

QString IrcContact::host() const
{
	return d->host;
}

void IrcContact::onSessionDestroyed()
{
	if (d->m_ref == 0)
		deleteLater();
}

void IrcContact::handleMode(const QString &who, const QString &mode, const QString &param)
{
	Q_UNUSED(who);
	Q_UNUSED(param);
	QChar action = mode[0];
	if (action == '+') {
		for (int i = 1; i < mode.size(); ++i)
			d->modes.insert(mode[i]);
	} else if (action == '-') {
		for (int i = 1; i < mode.size(); ++i)
			d->modes.remove(mode[i]);
	} else {
		foreach (QChar m, mode)
			d->modes.insert(m);
	}
}

void IrcContact::setAway(const QString &awayMsg)
{
	Status previous = status();
	d->awayMsg = awayMsg;
	emit statusChanged(status(), previous);
}

void IrcContact::setHost(const QString &host)
{
	static QRegExp ipRx("\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}$");
	if (d->hostMask == host || host.isEmpty())
		return;
	d->hostMask = host;
	int domainPos = host.indexOf('@');
	if (domainPos != -1) {
		++domainPos;
		int hostPos = ipRx.indexIn(host, domainPos) == -1 ? host.indexOf('.') : -1;
		if (hostPos != -1) {
			d->domain = host.mid(domainPos, hostPos-domainPos);
			d->host = host.mid(hostPos+1);
		} else {
			d->domain = host.mid(domainPos);
			d->host = QString();
		}
	} else {
		d->domain = QString();
		d->host = QString();
	}
}

} } // namespace qutim_sdk_0_3::irc

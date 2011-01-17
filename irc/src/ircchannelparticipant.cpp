/****************************************************************************
 *  ircchannelparticipant.cpp
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

#include "ircchannelparticipant.h"
#include "ircaccount_p.h"
#include "irccontact_p.h"
#include "ircchannel.h"

namespace qutim_sdk_0_3 {

namespace irc {

class IrcChannelParticipantPrivate
{
public:
	QPointer<IrcContact> contact;
	QPointer<IrcChannel> channel;
	IrcChannelParticipant::IrcParticipantFlags flags;
};

IrcChannelParticipant::IrcChannelParticipant(IrcChannel *channel, const QString &nick, const QString &host) :
	Buddy(channel->account()), d(new IrcChannelParticipantPrivate)
{
	d->channel = channel;
	d->contact = channel->account()->getContact(nick, host, true);
	d->contact->d->ref();
	setMenuOwner(d->contact);
	connect(d->contact, SIGNAL(nameChanged(QString,QString)), SIGNAL(nameChanged(QString,QString)));
	connect(d->contact, SIGNAL(quit(QString)), SIGNAL(quit(QString)));
	connect(d->contact, SIGNAL(avatarChanged(QString)), SIGNAL(avatarChanged(QString)));
	connect(d->contact, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
			SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)));
}

IrcChannelParticipant::~IrcChannelParticipant()
{
	if (d->contact)
		d->contact->d->deref();
}

bool IrcChannelParticipant::sendMessage(const Message &message)
{
	return d->contact->sendMessage(message);
}

QString IrcChannelParticipant::id() const
{
	return d->contact->id();
}

QString IrcChannelParticipant::name() const
{
	return d->contact->name();
}

QString IrcChannelParticipant::avatar() const
{
	return d->contact->avatar();
}

IrcContact *IrcChannelParticipant::contact()
{
	return d->contact;
}

const IrcContact *IrcChannelParticipant::contact() const
{
	return d->contact;
}

IrcChannel *IrcChannelParticipant::channel()
{
	return d->channel;
}

const IrcChannel *IrcChannelParticipant::channel() const
{
	return d->channel;
}

IrcAccount *IrcChannelParticipant::account()
{
	Q_ASSERT(qobject_cast<IrcAccount*>(ChatUnit::account()));
	return reinterpret_cast<IrcAccount*>(ChatUnit::account());
}

const IrcAccount *IrcChannelParticipant::account() const
{
	Q_ASSERT(qobject_cast<const IrcAccount*>(ChatUnit::account()));
	return reinterpret_cast<const IrcAccount*>(ChatUnit::account());
}

void IrcChannelParticipant::setFlag(QChar flag)
{
	if (flag == '+')
		d->flags |= Voice;
	else if (flag == '%')
		d->flags |= HalfOp;
	else if (flag == '@')
		d->flags |= Op;
}

void IrcChannelParticipant::setMode(QChar mode)
{
	if (mode == 'v')
		d->flags |= Voice;
	else if (mode == 'h')
		d->flags |= HalfOp;
	else if (mode == 'o')
		d->flags |= Op;
}

void IrcChannelParticipant::removeMode(QChar mode)
{
	if (mode == 'v')
		d->flags ^= Voice;
	else if (mode == 'h')
		d->flags ^= HalfOp;
	else if (mode == 'o')
		d->flags ^= Op;
}

QString IrcChannelParticipant::hostMask() const
{
	return d->contact->hostMask();
}

QString IrcChannelParticipant::domain() const
{
	return d->contact->domain();
}

QString IrcChannelParticipant::host() const
{
	return d->contact->host();
}

} } // namespace qutim_sdk_0_3::irc

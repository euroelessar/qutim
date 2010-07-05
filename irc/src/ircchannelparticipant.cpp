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
};

IrcChannelParticipant::IrcChannelParticipant(IrcChannel *channel, const QString &nick) :
	Buddy(channel->account()), d(new IrcChannelParticipantPrivate)
{
	d->contact = channel->account()->getContact(nick, true);
	d->contact->d->ref();
	setMenuOwner(d->contact);
	connect(d->contact, SIGNAL(nameChanged(QString)), SIGNAL(nameChanged(QString)));
	connect(d->contact, SIGNAL(quit(QString)), SIGNAL(quit(QString)));
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

IrcContact *IrcChannelParticipant::contact()
{
	return d->contact;
}

const IrcContact *IrcChannelParticipant::contact() const
{
	return d->contact;
}
} } // namespace qutim_sdk_0_3::irc

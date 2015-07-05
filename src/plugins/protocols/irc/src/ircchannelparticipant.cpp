/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

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
	d->contact.data()->ref();
	setMenuOwner(d->contact.data());
	connect(d->contact.data(), SIGNAL(nameChanged(QString,QString)), SIGNAL(nameChanged(QString,QString)));
	connect(d->contact.data(), SIGNAL(quit(QString)), SIGNAL(quit(QString)));
	connect(d->contact.data(), SIGNAL(avatarChanged(QString)), SIGNAL(avatarChanged(QString)));
	connect(d->contact.data(), SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
			SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)));
}

IrcChannelParticipant::~IrcChannelParticipant()
{
	if (d->contact)
		d->contact.data()->deref();
}

bool IrcChannelParticipant::sendMessage(const Message &message)
{
	return d->contact.data()->sendMessage(message);
}

QString IrcChannelParticipant::id() const
{
	return d->contact.data()->id();
}

QString IrcChannelParticipant::name() const
{
	return d->contact.data()->name();
}

QString IrcChannelParticipant::avatar() const
{
	return d->contact.data()->avatar();
}

Status IrcChannelParticipant::status() const
{
	return d->contact.data()->status();
}

IrcContact *IrcChannelParticipant::contact()
{
	return d->contact.data();
}

const IrcContact *IrcChannelParticipant::contact() const
{
	return d->contact.data();
}

IrcChannel *IrcChannelParticipant::channel()
{
	return d->channel.data();
}

const IrcChannel *IrcChannelParticipant::channel() const
{
	return d->channel.data();
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
	return d->contact.data()->hostMask();
}

QString IrcChannelParticipant::hostUser() const
{
	return d->contact.data()->hostUser();
}

QString IrcChannelParticipant::domain() const
{
	return d->contact.data()->domain();
}

QString IrcChannelParticipant::host() const
{
	return d->contact.data()->host();
}

} } // namespace qutim_sdk_0_3::irc


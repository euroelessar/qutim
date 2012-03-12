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

#include "irccontact_p.h"
#include "ircaccount_p.h"
#include "ircconnection.h"
#include "ircavatar.h"
#include <qutim/chatsession.h>
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
	setHostMask(host);
	//IrcAvatar::instance()->requestAvatar(this);
	if (account->d->conn->autoRequestWhois())
		account->send("WHOIS :" + nick, false);
}

IrcContact::~IrcContact()
{
	Q_ASSERT(d->m_ref == 0);
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
	foreach (const QString &line, message.text().split(QLatin1Char('\n')))
		account()->send(QString("PRIVMSG %1 :%2").arg(d->nick).arg(line));
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

QString IrcContact::hostUser() const
{
	return d->hostUser.isEmpty() ? "~" + d->nick : d->hostUser;
}

QString IrcContact::domain() const
{
	return d->domain;
}

QString IrcContact::host() const
{
	return d->host;
}

QString IrcContact::realName() const
{
	return d->realName;
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

void IrcContact::setHostMask(const QString &host)
{
	if (d->hostMask == host || host.isEmpty())
		return;
	d->hostMask = host;
	int pos = host.indexOf('@');
	if (pos != -1) {
		d->hostUser = host.mid(0, pos);
		setHost(host, ++pos);
	} else {
		d->hostUser = host;
		d->domain = QString();
		d->host = QString();
	}
}

void IrcContact::setHost(const QString &host, int pos)
{
	static QRegExp ipRx("\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}$");
	int hostPos = ipRx.indexIn(host, pos) == -1 ? host.indexOf('.') : -1;
	if (hostPos != -1) {
		d->domain = host.mid(pos, hostPos-pos);
		d->host = host.mid(hostPos+1);
	} else {
		d->domain = host.mid(pos);
		d->host = QString();
	}
}

void IrcContact::setHostUser(const QString &user)
{
	d->hostUser = user;
}

void IrcContact::setRealName(const QString &name)
{
	d->realName = name;
}

} } // namespace qutim_sdk_0_3::irc


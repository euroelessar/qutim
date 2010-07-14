/****************************************************************************
 *  icqaccount.cpp
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

#include "ircaccount_p.h"
#include "ircprotocol.h"
#include "ircconnection.h"
#include "ircchannel.h"
#include "ircchannelparticipant.h"
#include <qutim/status.h>
#include <qutim/messagesession.h>
#include <QTime>

namespace qutim_sdk_0_3 {

namespace irc {

QHash<QString, QString> IrcAccountPrivate::logMsgColors;

IrcContact *IrcAccountPrivate::newContact(const QString &nick)
{
	IrcContact *contact = new IrcContact(q, nick);
	q->connect(contact, SIGNAL(destroyed()), SLOT(onContactRemoved()));
	q->connect(contact, SIGNAL(nameChanged(QString)), SLOT(onContactNickChanged(QString)));
	contacts.insert(nick, contact);
	return contact;
}

IrcChannel *IrcAccountPrivate::newChannel(const QString &name)
{
	IrcChannel *channel = new IrcChannel(q, name);
	channels.insert(name, channel);
	return channel;
}

IrcAccount::IrcAccount(const QString &network) :
	Account(network, IrcProtocol::instance()), d(new IrcAccountPrivate)
{
	d->q = this;
	d->conn = new IrcConnection(this, this);
}

IrcAccount::~IrcAccount()
{
}

void IrcAccount::setStatus(Status status)
{
	Status current = this->status();
	if (status == Status::Connecting)
		return;
	// Prepare status.
	if (current == Status::Connecting) {
		status.setType(current.text().isEmpty() ? Status::Online : Status::Away);
		status.setText(current.text());
	} else if (status == Status::Offline || status == Status::Online) {
		status.setText(QString());
	} else if (status == Status::Invisible || status == Status::FreeChat) {
		status.setType(Status::Online);
		status.setText(QString());
	} else {
		if (status != Status::Away)
			status.setType(Status::Away);
		if (status.text().isEmpty())
			status.setText(tr("Away"));
	}
	// Send status.
	if (status == Status::Offline) {
		if (d->conn->isConnected())
			d->conn->disconnectFromHost(false);
		foreach (IrcChannel *channel, d->channels)
			channel->leave(true);
	} else {
		if (current == Status::Offline) {
			status.setType(Status::Connecting);
			d->conn->connectToNetwork();
		} else if (current == Status::Away && status == Status::Online) {
			// It is a little weird but the following command sets status to Online.
			d->conn->send("AWAY");
		}
		if (status.type() == Status::Away)
			d->conn->send(QString("AWAY %1").arg(status.text()));
	}
	status.initIcon("irc");
	emit statusChanged(status);
	Account::setStatus(status);
}

QString IrcAccount::name() const
{
	return d->conn->nick();
}

ChatUnit *IrcAccount::getUnitForSession(ChatUnit *unit)
{
	if (IrcChannelParticipant *participant = qobject_cast<IrcChannelParticipant*>(unit)) {
		return participant->contact();
	}
	return unit;
}

ChatUnit *IrcAccount::getUnit(const QString &name, bool create)
{
	if (name.startsWith('#') || name.startsWith('&'))
		return 0;
	return getContact(name, true);
}

IrcChannel *IrcAccount::getChannel(const QString &name, bool create)
{
	IrcChannel *channel = d->channels.value(name);
	if (create && !channel) {
		channel = d->newChannel(name);
		emit conferenceCreated(channel);
	}
	return channel;
}

IrcContact *IrcAccount::getContact(const QString &nick, bool create)
{
	IrcContact *contact = d->contacts.value(nick);
	if (create && !contact)
		contact = d->newContact(nick);
	return contact;
}

void IrcAccount::send(const QString &command) const
{
	d->conn->send(command);
}

void IrcAccount::setName(const QString &name) const
{
	send(QString("NICK %1").arg(name));
}

IrcProtocol *IrcAccount::protocol()
{
	Q_ASSERT(qobject_cast<IrcProtocol*>(Account::protocol()));
	return reinterpret_cast<IrcProtocol*>(Account::protocol());
}

const IrcProtocol *IrcAccount::protocol() const
{
	Q_ASSERT(qobject_cast<const IrcProtocol*>(Account::protocol()));
	return reinterpret_cast<const IrcProtocol*>(Account::protocol());
}

ChatSession *IrcAccount::activeSession() const
{
	ChatSession *session = protocol()->activeSession();
	return session && session->getUnit()->account() == this ? session : 0;
}

void IrcAccount::log(const QString &msg, bool addToActiveSession, const QString &type)
{
	// Add to an active session.
	if (addToActiveSession) {
		ChatSession *session = activeSession();
		if (session) {
			Message message(msg);
			message.setChatUnit(session->getUnit());
			message.setProperty("service", true);
			message.setTime(QDateTime::currentDateTime());
			session->appendMessage(message);
		}
	}
	// Add to the account console.
	QString str = QString("[%1] ").arg(QTime::currentTime().toString(Qt::SystemLocaleShortDate));
	if (!type.isEmpty()) {
		QString color = d->logMsgColors.value(type);
		if (!color.isEmpty())
			str += QString("<font color='%1'>[%2] %3 </font>").arg(color).arg(type).arg(msg);
		else
			str += QString("[%1] %2").arg(type).arg(msg);
	} else {
		str += msg;
	}
	if (d->consoleForm)
		d->consoleForm->appendMessage(str);
	if (!d->log.isEmpty())
		d->log += "<br>";
	d->log += str;
}

void IrcAccount::registerLogMsgColor(const QString &type, const QString &color)
{
	IrcAccountPrivate::logMsgColors.insert(type, color);
}

void IrcAccount::updateSettings()
{
	d->conn->loadSettings();
}

void IrcAccount::showConsole()
{
	if (d->consoleForm) {
		d->consoleForm->raise();
	} else {
		d->consoleForm = new IrcConsoleFrom(this, d->log);
		d->consoleForm->setAttribute(Qt::WA_DeleteOnClose);
		centerizeWidget(d->consoleForm);
		d->consoleForm->show();
	}
}

bool IrcAccount::event(QEvent *ev)
{
	return Account::event(ev);
}

void IrcAccountPrivate::removeContact(IrcContact *contact)
{
	QHash<QString, IrcContact *>::iterator itr = contacts.begin();
	QHash<QString, IrcContact *>::iterator endItr = contacts.end();
	while (itr != endItr) {
		if (*itr == contact) {
			contacts.erase(itr);
			break;
		}
		++itr;
	}
	Q_ASSERT(itr != endItr);
}

void IrcAccount::onContactRemoved()
{
	d->removeContact(reinterpret_cast<IrcContact*>(sender()));
}

void IrcAccount::onContactNickChanged(const QString &nick)
{
	Q_ASSERT(qobject_cast<IrcContact*>(sender()));
	IrcContact *contact = reinterpret_cast<IrcContact*>(sender());
	d->removeContact(contact);
	if (d->contacts.contains(nick)) {
		// It should never happen but anyway:
		d->contacts.take(nick)->deleteLater();
	}
	Q_ASSERT(contact->id() == nick);
	d->contacts.insert(nick, contact);
}

} } // namespace qutim_sdk_0_3::irc

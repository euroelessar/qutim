/****************************************************************************
 *  ircchannel.cpp
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

#include "ircchannel.h"
#include "ircchannelparticipant.h"
#include "ircaccount.h"
#include <qutim/messagesession.h>
#include <QDateTime>

namespace qutim_sdk_0_3 {

namespace irc {

typedef QSharedPointer<IrcChannelParticipant> ParticipantPointer;

class IrcChannelPrivate
{
public:
	ParticipantPointer me;
	QString name;
	QHash<QString, ParticipantPointer> users;
	QString topic;
};

IrcChannel::IrcChannel(IrcAccount *account, const QString &name) :
	Conference(account), d(new IrcChannelPrivate)
{
	d->name = name;
}

IrcChannel::~IrcChannel()
{
}

Buddy *IrcChannel::me() const
{
	return d->me.data();
}

void IrcChannel::join()
{
	join(QString());
}

void IrcChannel::join(const QString &pass)
{
	QString cmd;
	if (!pass.isEmpty())
		cmd = QString("JOIN %1 :%2").arg(d->name).arg(pass);
	else
		cmd = QString("JOIN %1").arg(d->name);
	account()->send(cmd);
}

void IrcChannel::leave()
{
	account()->send(QString("PART %1").arg(d->name));
}

QString IrcChannel::id() const
{
	return d->name;
}

bool IrcChannel::sendMessage(const Message &message)
{
	Status::Type status = account()->status().type();
	if (status == Status::Connecting || status == Status::Offline)
		return false;
	account()->send(QString("PRIVMSG %1 :%2").arg(d->name).arg(message.text()));
	return true;
}

QString IrcChannel::topic() const
{
	return d->topic;
}

const IrcAccount *IrcChannel::account() const
{
	Q_ASSERT(qobject_cast<const IrcAccount*>(Conference::account()));
	return reinterpret_cast<const IrcAccount*>(Conference::account());
}

IrcAccount *IrcChannel::account()
{
	Q_ASSERT(qobject_cast<IrcAccount*>(Conference::account()));
	return reinterpret_cast<IrcAccount*>(Conference::account());
}

IrcChannelParticipant *IrcChannel::participant(const QString &nick)
{
	return d->users.value(nick).data();
}

QList<IrcChannelParticipant*> IrcChannel::participants()
{
	QList<IrcChannelParticipant*> users;
	foreach (const QSharedPointer<IrcChannelParticipant> &user, d->users)
		users << user.data();
	return users;
}

void IrcChannel::onMyNickChanged(const QString &nick)
{
	ChatSession *session = ChatLayer::instance()->getSession(this, false);
	if (session) {
		addSystemMessage(QT_TRANSLATE_NOOP("IrcChannel", "You are now known as %1")
						 .toString()
						 .arg(nick),
						 session);
	}
}

void IrcChannel::onParticipantNickChanged(const QString &nick)
{
	QHash<QString, ParticipantPointer>::iterator itr = d->users.begin();
	QHash<QString, ParticipantPointer>::iterator endItr = d->users.end();
	ParticipantPointer user;
	QString oldNick;
	while (itr != endItr) {
		if (itr->data() == sender()) {
			user = *itr;
			oldNick = itr.key();
			d->users.erase(itr);
			break;
		}
		++itr;
	}
	if (!user)
		return;
	if (d->users.contains(nick))
		d->users.remove(nick);
	d->users.insert(nick, user);
	ChatSession *session = ChatLayer::instance()->getSession(this, false);
	if (session) {
		addSystemMessage(QT_TRANSLATE_NOOP("IrcChannel", "%1 are now known as %2")
						 .toString()
						 .arg(oldNick)
						 .arg(nick),
						 session);
	}
}

void IrcChannel::onContactQuit(const QString &message)
{
	Q_ASSERT(qobject_cast<IrcChannelParticipant*>(sender()));
	IrcChannelParticipant *user = reinterpret_cast<IrcChannelParticipant*>(sender());
	handlePart(user->name(), message);
}

void IrcChannel::handleUserList(const QStringList &users)
{
	ChatSession *session = ChatLayer::instance()->getSession(this, true);
	QString myNick = account()->name();
	foreach (QString userNick, users) {
		Q_ASSERT(!userNick.isEmpty());
		QChar first = userNick.at(0);
		if (first == '+' || first == '@') // TODO: set mode to user
			userNick = userNick.mid(1);
		bool isMe = userNick == myNick;
		if ((isMe && me()) || (!isMe && d->users.contains(userNick)))
			continue;
		ParticipantPointer user = ParticipantPointer(new IrcChannelParticipant(this, userNick));
		if (isMe) {
			connect(user.data(), SIGNAL(nameChanged(QString)), SLOT(onMyNickChanged(QString)));
			d->me = user;
		} else {
			connect(user.data(), SIGNAL(nameChanged(QString)), SLOT(onParticipantNickChanged(QString)));
			connect(user.data(), SIGNAL(quit(QString)), SLOT(onContactQuit(QString)));
			d->users.insert(userNick, user);
		}
		if (session)
			session->addContact(user.data());
	}
	session->activate();
}

void IrcChannel::handleJoin(const QString &nick, const QString &host)
{
	if (!d->users.contains(nick)) {
		ParticipantPointer user = ParticipantPointer(new IrcChannelParticipant(this, nick));
		connect(user.data(), SIGNAL(nameChanged(QString)), SLOT(onParticipantNickChanged(QString)));
		connect(user.data(), SIGNAL(quit(QString)), SLOT(onContactQuit(QString)));
		d->users.insert(nick, user);
		ChatSession *session = ChatLayer::instance()->getSession(this, false);
		if (session) {
			session->addContact(user.data());
			addSystemMessage(QT_TRANSLATE_NOOP("IrcChannel", "%1 (%2) has joined the channel")
							 .toString()
							 .arg(nick)
							 .arg(host),
							 session);
		}
	} else {
		debug() << nick << "already presents in" << d->name;
	}
}

void IrcChannel::handlePart(const QString &nick, const QString &leaveMessage)
{
	if (nick == account()->name())
		return;
	if (ParticipantPointer user = d->users.take(nick)) {
		ChatSession *session = ChatLayer::instance()->getSession(this, false);
		if (session) {
			session->removeContact(user.data());
			QString text;
			if (!leaveMessage.isEmpty())
				text = QT_TRANSLATE_NOOP("IrcChannel", "%1 has left this channel (%2)")
						.toString()
						.arg(nick)
						.arg(leaveMessage);
			else
				text = QT_TRANSLATE_NOOP("IrcChannel", "%1 has left this channel")
						.toString()
						.arg(nick);
			addSystemMessage(text, session);
		}
	} else {
		debug() << nick << "does not present in" << d->name;
	}
}

void IrcChannel::handleTopic(const QString &topic)
{
	d->topic = topic;
	emit topicChanged(topic);
	ChatSession *session = ChatLayer::instance()->getSession(this, false);
	if (session)
		addSystemMessage(QT_TRANSLATE_NOOP("IrcChannel", "The channel topic is \"%1\"")
						 .toString()
						 .arg(topic),
						 session);
}

void IrcChannel::handleTopicInfo(const QString &user, const QString &timeStr)
{
	ChatSession *session = ChatLayer::instance()->getSession(this, false);
	if (session) {
		int time_t = timeStr.toInt();
		if (!time_t)
			return;
		QDateTime time = QDateTime::fromTime_t(time_t);
		addSystemMessage(QT_TRANSLATE_NOOP("IrcChannel", "The topic was set by %1 on %2.")
						 .toString()
						 .arg(user)
						 .arg(time.toString(Qt::SystemLocaleShortDate)),
						 session);
	}
}

void IrcChannel::addSystemMessage(const QString &message, ChatSession *session)
{
	Message msg(message);
	msg.setChatUnit(this);
	msg.setProperty("service", true);
	msg.setTime(QDateTime::currentDateTime());
	session->appendMessage(msg);
}

} } // namespace qutim_sdk_0_3::irc

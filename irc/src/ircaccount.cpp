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
#include <qutim/event.h>
#include <qutim/dataforms.h>
#include <QTime>

namespace qutim_sdk_0_3 {

namespace irc {

QHash<QString, QString> IrcAccountPrivate::logMsgColors;

IrcContact *IrcAccountPrivate::newContact(const QString &nick)
{
	IrcContact *contact = new IrcContact(q, nick);
	q->connect(contact, SIGNAL(destroyed()), SLOT(onContactRemoved()));
	q->connect(contact, SIGNAL(nameChanged(QString,QString)),
			   SLOT(onContactNickChanged(QString)));
	contacts.insert(nick, contact);
	return contact;
}

IrcChannel *IrcAccountPrivate::newChannel(const QString &name)
{
	IrcChannel *channel = new IrcChannel(q, name);
	channels.insert(name, channel);
	return channel;
}


template <class T>
QVariantList IrcAccountPrivate::toVariant(const T &list)
{
	QVariantList items;
	foreach (const IrcBookmark &bookmark, list) {
		QVariantMap item;
		QString name = bookmark.getName();
		item.insert("name", name);
		QVariantMap data;
		data.insert(QT_TRANSLATE_NOOP("IRC", "Channel"), bookmark.channel);
		if (bookmark.autojoin)
			data.insert(QT_TRANSLATE_NOOP("IRC", "Autojoin"), QT_TRANSLATE_NOOP("IRC", "Yes").toString());
		item.insert("fields", data);
		items.append(item);
	}
	return items;
}

void IrcAccountPrivate::updateRecent(const QString &channel, const QString &password)
{
	QList<IrcBookmark>::iterator itr = recent.begin();
	QList<IrcBookmark>::iterator endItr = recent.end();
	while (itr != endItr) {
		if (itr->channel == channel) {
			recent.erase(itr);
			break;
		}
		++itr;
	}
	IrcBookmark bookmark = { QString(), channel, password, false };
	recent.push_front(bookmark);
	if (recent.size() > 10)
		recent = recent.mid(0, 10);

	Config cfg = q->config();
	cfg.remove("recent");
	cfg.beginArray("recent");
	for (int i = 0; i < recent.size(); ++i) {
		cfg.setArrayIndex(i);
		saveBookmarkToConfig(cfg, recent.at(i));
	}
	cfg.endArray();
}

void IrcAccountPrivate::saveBookmarkToConfig(Config &cfg, const IrcBookmark &bookmark)
{
	if (!bookmark.name.isEmpty())
		cfg.setValue("name", bookmark.name);
	cfg.setValue("channel", bookmark.channel);
	if (!bookmark.password.isEmpty())
		cfg.setValue("password", bookmark.password, Config::Crypted);
	cfg.setValue("autojoin", bookmark.autojoin);
}

IrcBookmark IrcAccountPrivate::loadBookmarkFromConfig(Config &cfg)
{
	IrcBookmark bookmark;
	bookmark.name = cfg.value("name", QString());
	bookmark.channel = cfg.value("channel", QString());
	bookmark.password = cfg.value("password", QString(), Config::Crypted);
	bookmark.autojoin = cfg.value("autojoin", false);
	return bookmark;
}

IrcAccount::IrcAccount(const QString &network) :
	Account(network, IrcProtocol::instance()), d(new IrcAccountPrivate)
{
	d->q = this;
	d->conn = new IrcConnection(this, this);
	d->eventTypes.groupChatFields = Event::registerType("groupchat-fields");
	d->eventTypes.groupChatJoin = Event::registerType("groupchat-join");
	d->eventTypes.bookmarkList = Event::registerType("groupchat-bookmark-list");
	d->eventTypes.bookmarkRemove = Event::registerType("groupchat-bookmark-remove");
	d->eventTypes.bookmarkSave = Event::registerType("groupchat-bookmark-save");

	Config cfg = config("bookmarks");
	foreach (const QString &name, cfg.childGroups()) {
		cfg.beginGroup(name);
		IrcBookmark bookmark = d->loadBookmarkFromConfig(cfg);
		d->bookmarks.insert(name, bookmark);
		cfg.endGroup();
	}

	cfg = config();
	cfg.beginArray("recent");
	for (int i = 0, size = cfg.arraySize(); i < size; ++i) {
		cfg.setArrayIndex(i);
		IrcBookmark bookmark = d->loadBookmarkFromConfig(cfg);
		if (!bookmark.channel.isEmpty())
			d->recent << bookmark;
	}
	cfg.endArray();
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
	Account::setStatus(status);
	emit statusChanged(status, current);
}

QString IrcAccount::name() const
{
	return d->conn->nick();
}

QString IrcAccount::avatar()
{
	return d->avatar;
}

void IrcAccount::setAvatar(const QString &avatar)
{
	d->avatar = avatar;
	emit avatarChanged(avatar);
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

void IrcAccount::send(const QString &command, IrcCommandAlias::Type aliasType, const QHash<QChar, QString> &extParams) const
{
	d->conn->send(command, aliasType, extParams);
}

void IrcAccount::sendCtpcRequest(const QString &contact, const QString &cmd, const QString &params)
{
	d->conn->sendCtpcRequest(contact, cmd, params);
}

void IrcAccount::sendCtpcReply(const QString &contact, const QString &cmd, const QString &params)
{
	d->conn->sendCtpcReply(contact, cmd, params);
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
	QString plainText;
	QString html = IrcProtocol::ircFormatToHtml(msg, &plainText);
	// Add to an active session.
	if (addToActiveSession) {
		ChatSession *session = activeSession();
		if (session) {
			Message message(plainText);
			message.setChatUnit(session->getUnit());
			message.setProperty("service", true);
			message.setProperty("html", html);
			message.setTime(QDateTime::currentDateTime());
			session->appendMessage(message);
		}
	}
	// Add to the account console.
	QString str = QString("[%1] ").arg(QTime::currentTime().toString(Qt::SystemLocaleShortDate));
	if (!type.isEmpty()) {
		QString color = d->logMsgColors.value(type);
		if (!color.isEmpty())
			str += QString("<font color='%1'>[%2] %3 </font>").arg(color).arg(type).arg(html);
		else
			str += QString("[%1] %2").arg(type).arg(html);
	} else {
		str += html;
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

void IrcAccount::showChannelList()
{
	if (d->channelListForm) {
		d->channelListForm->raise();
	} else {
		d->channelListForm = new IrcChannelListForm(this);
		d->channelListForm->setAttribute(Qt::WA_DeleteOnClose);
		centerizeWidget(d->channelListForm);
		d->channelListForm->show();
	}
}

bool IrcAccount::event(QEvent *ev)
{
	if (ev->type() == Event::eventType()) {
		qutim_sdk_0_3::Event *event = static_cast<qutim_sdk_0_3::Event*>(ev);
		if (event->id == d->eventTypes.groupChatFields) {
			QString name =  event->args[1].toString();
			bool isBookmark = event->args[2].toBool();
			IrcBookmark bookmark = d->bookmarks.value(name);
			if (bookmark.channel.isEmpty()) {
				QList<IrcBookmark>::iterator itr = d->recent.begin();
				QList<IrcBookmark>::iterator endItr = d->recent.end();
				while (itr != endItr) {
					if (itr->channel == name) {
						bookmark = *itr;
						break;
					}
					++itr;
				}
			}
			DataItem item(QT_TRANSLATE_NOOP("IRC", "Join channel"));
			if (isBookmark)
				item.addSubitem(DataItem("name", QT_TRANSLATE_NOOP("IRC", "Name"), bookmark.name));
			{
				DataItem channelItem("channel", QT_TRANSLATE_NOOP("IRC", "Channel"),
									 bookmark.channel.isEmpty() ? "#" : bookmark.channel);
				channelItem.setProperty("validator", QRegExp("^(#|&|!|\\+)[^\\s\\0007,]{1,50}"));
				item.addSubitem(channelItem);
			}
			{
				DataItem passwordItem("password", QT_TRANSLATE_NOOP("IRC", "Password"), bookmark.password);
				passwordItem.setProperty("passwordMode", true);
				item.addSubitem(passwordItem);
			}
			if (isBookmark)
				item.addSubitem(DataItem("autojoin", QT_TRANSLATE_NOOP("IRC", "Auto-join"), bookmark.autojoin));
			event->args[0] = qVariantFromValue(item);
			return true;
		} else if (event->id == d->eventTypes.groupChatJoin) {
			DataItem item = event->at<DataItem>(0);
			Q_ASSERT(!item.isNull());
			QString channelName = item.subitem("channel").data<QString>();
			if (channelName.length() <= 1)
				return false;
			IrcChannel *channel = getChannel(channelName, true);
			channel->join(item.subitem("password").data<QString>());
			ChatLayer::instance()->getSession(channel, true)->activate();
			return true;
		} else if (event->id == d->eventTypes.bookmarkList) {
			event->args[0] = d->toVariant(d->bookmarks);
			event->args[1] = d->toVariant(d->recent);
			return true;
		} else if (event->id == d->eventTypes.bookmarkRemove) {
			QString name = event->at<QString>(0);
			d->bookmarks.remove(name);
			return true;
		} else if (event->id == d->eventTypes.bookmarkSave) {
			qutim_sdk_0_3::DataItem item = event->at<qutim_sdk_0_3::DataItem>(0);
			QString oldName = event->at<QString>(1);
			d->bookmarks.remove(oldName);
			IrcBookmark bookmark;
			bookmark.channel = item.subitem("channel").data<QString>();
			if (bookmark.channel.length() <= 1)
				return false;
			bookmark.name = item.subitem("name").data<QString>();
			bookmark.password = item.subitem("password").data<QString>();
			bookmark.autojoin = item.subitem("autojoin").data<bool>();
			d->bookmarks.insert(bookmark.getName(), bookmark);

			Config cfg = config("bookmarks");
			cfg.remove(oldName);
			cfg.beginGroup(bookmark.getName());
			d->saveBookmarkToConfig(cfg, bookmark);
			cfg.endGroup();
			return true;
		}
	}
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

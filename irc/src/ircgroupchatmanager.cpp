/****************************************************************************
 *  ircgroupchatmanager.cpp
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

#include "ircgroupchatmanager.h"
#include "ircchannel.h"
#include "ircaccount_p.h"

namespace qutim_sdk_0_3 {
namespace irc {

IrcGroupChatManager::IrcGroupChatManager(IrcAccount *account) :
	GroupChatManager(account)
{
	Config cfg = account->config();
	cfg.beginGroup("bookmarks");
	foreach (const QString &name, cfg.childGroups()) {
		cfg.beginGroup(name);
		IrcBookmark bookmark = loadBookmarkFromConfig(cfg);
		cfg.endGroup();
		if (bookmark.channel.isEmpty())
			continue;
		m_bookmarks.insert(name, bookmark);

		if (bookmark.autojoin) {
			IrcChannel *channel = account->getChannel(bookmark.channel, true);
			channel->setAutoJoin(true);
			channel->setBookmarkName(bookmark.name);
		}
	}
	cfg.endGroup();

	cfg.beginArray("recent");
	for (int i = 0, size = cfg.arraySize(); i < size; ++i) {
		cfg.setArrayIndex(i);
		IrcBookmark bookmark = loadBookmarkFromConfig(cfg);
		if (!bookmark.channel.isEmpty())
			m_recent << bookmark;
	}
	cfg.endArray();
}

DataItem IrcGroupChatManager::fields(IrcBookmark bookmark, bool isBookmark) const
{
	DataItem item(bookmark.getName());
	{
		DataItem nameItem("name", QT_TRANSLATE_NOOP("IRC", "Name"), bookmark.getName());
		if (!isBookmark)
			nameItem.setProperty("showInBookmarkInfo", false);
		item.addSubitem(nameItem);
	}
	{
		DataItem channelItem("channel", QT_TRANSLATE_NOOP("IRC", "Channel"),
							 bookmark.channel.isEmpty() ? "#" : bookmark.channel);
		channelItem.setProperty("validator", QRegExp("^(#|&|!|\\+)[^\\s\\0007,]{1,50}"));
		channelItem.setProperty("mandatory", true);
		item.addSubitem(channelItem);
	}
	{
		DataItem passwordItem("password", QT_TRANSLATE_NOOP("IRC", "Password"), bookmark.password);
		passwordItem.setProperty("password", true);
		passwordItem.setProperty("showInBookmarkInfo", false);
		item.addSubitem(passwordItem);
	}
	{
		DataItem autoJoinItem("autojoin", QT_TRANSLATE_NOOP("IRC", "Auto-join"), bookmark.autojoin);
		if (!isBookmark)
			autoJoinItem.setProperty("showInBookmarkInfo", false);
		item.addSubitem(autoJoinItem);
	}
	return item;
}

DataItem IrcGroupChatManager::fields() const
{
	return fields(IrcBookmark());
}

bool IrcGroupChatManager::join(const DataItem &fields)
{
	QString channelName = fields.subitem("channel").data<QString>();
	if (channelName.length() <= 1)
		return false;
	IrcChannel *channel = account()->getChannel(channelName, true);
	channel->setBookmarkName(fields.subitem("name").data<QString>());
	channel->join(fields.subitem("password").data<QString>());
	return true;
}

bool IrcGroupChatManager::storeBookmark(const DataItem &fields, const DataItem &oldFields)
{
	QString oldName = oldFields.subitem("name").data<QString>();
	m_bookmarks.remove(oldName);
	IrcBookmark bookmark;
	bookmark.channel = fields.subitem("channel").data<QString>();
	if (bookmark.channel.length() <= 1)
		return false;
	bookmark.name = fields.subitem("name").data<QString>();
	bookmark.password = fields.subitem("password").data<QString>();
	bookmark.autojoin = fields.subitem("autojoin").data<bool>();
	m_bookmarks.insert(bookmark.getName(), bookmark);

	Config cfg = account()->config("bookmarks");
	cfg.remove(oldName);
	cfg.beginGroup(bookmark.getName());
	saveBookmarkToConfig(cfg, bookmark);
	cfg.endGroup();
	return true;
}

bool IrcGroupChatManager::removeBookmark(const DataItem &fields)
{
	QString name = fields.subitem("name").data<QString>();
	m_bookmarks.remove(name);
	Config cfg = account()->config("bookmarks");
	cfg.remove(name);
	return true;
}

QList<DataItem> IrcGroupChatManager::bookmarks() const
{
	QList<DataItem> bookmarks;
	foreach (const IrcBookmark &bookmark, m_bookmarks)
		bookmarks << fields(bookmark);
	return bookmarks;
}

QList<DataItem> IrcGroupChatManager::recent() const
{
	QList<DataItem> bookmarks;
	foreach (const IrcBookmark &bookmark, m_recent)
		bookmarks << fields(bookmark, true);
	return bookmarks;
}

void IrcGroupChatManager::updateRecent(const QString &channel, const QString &password)
{
	QList<IrcBookmark>::iterator itr = m_recent.begin();
	QList<IrcBookmark>::iterator endItr = m_recent.end();
	while (itr != endItr) {
		if (itr->channel == channel) {
			m_recent.erase(itr);
			break;
		}
		++itr;
	}
	IrcBookmark bookmark = { QString(), channel, password, false };
	m_recent.push_front(bookmark);
	if (m_recent.size() > 10)
		m_recent = m_recent.mid(0, 10);

	Config cfg = account()->config();
	cfg.remove("recent");
	cfg.beginArray("recent");
	for (int i = 0; i < m_recent.size(); ++i) {
		cfg.setArrayIndex(i);
		saveBookmarkToConfig(cfg, m_recent.at(i));
	}
	cfg.endArray();
}

void IrcGroupChatManager::saveBookmarkToConfig(Config &cfg, const IrcBookmark &bookmark)
{
	if (!bookmark.name.isEmpty())
		cfg.setValue("name", bookmark.name);
	cfg.setValue("channel", bookmark.channel);
	if (!bookmark.password.isEmpty())
		cfg.setValue("password", bookmark.password, Config::Crypted);
	cfg.setValue("autojoin", bookmark.autojoin);
}

IrcBookmark IrcGroupChatManager::loadBookmarkFromConfig(Config &cfg)
{
	IrcBookmark bookmark;
	bookmark.name = cfg.value("name", QString());
	bookmark.channel = cfg.value("channel", QString());
	bookmark.password = cfg.value("password", QString(), Config::Crypted);
	bookmark.autojoin = cfg.value("autojoin", false);
	return bookmark;
}


} // namespace irc
} // namespace qutim_sdk_0_3

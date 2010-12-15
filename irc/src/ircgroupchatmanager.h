/****************************************************************************
 *  ircgroupchatmanager.h
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

#ifndef IRCGROUPCHATMANAGER_H
#define IRCGROUPCHATMANAGER_H

#include "ircglobal.h"
#include <qutim/groupchatmanager.h>
#include <qutim/dataforms.h>
#include <qutim/config.h>

namespace qutim_sdk_0_3 {
namespace irc {

class IrcAccount;

struct IrcBookmark
{
	QString getName() const { return name.isEmpty() ? channel : name; }
	QString name;
	QString channel;
	QString password;
	bool autojoin;
};

class IrcGroupChatManager : public GroupChatManager
{
	Q_INTERFACES(qutim_sdk_0_3::GroupChatManager)
public:
	IrcGroupChatManager(IrcAccount *account);
	IrcAccount *account() const { return reinterpret_cast<IrcAccount*>(GroupChatManager::account()); }
	DataItem fields(IrcBookmark bookmark, bool isBookmark = true) const;
	virtual DataItem fields() const;
	virtual bool join(const DataItem &fields);
	virtual bool storeBookmark(const DataItem &fields, const DataItem &oldFields);
	virtual bool removeBookmark(const DataItem &fields);
	virtual QList<DataItem> bookmarks() const;
	virtual QList<DataItem> recent() const;
	void updateRecent(const QString &channel, const QString &password);
private:
	void saveBookmarkToConfig(Config &cfg, const IrcBookmark &bookmark);
	IrcBookmark loadBookmarkFromConfig(Config &cfg);
	QHash<QString, IrcBookmark> m_bookmarks;
	QList<IrcBookmark> m_recent;
};

} // namespace irc
} // namespace qutim_sdk_0_3

#endif // IRCGROUPCHATMANAGER_H

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

#ifndef IRCGROUPCHATMANAGER_H
#define IRCGROUPCHATMANAGER_H

#include "ircglobal.h"
#include <qutim/groupchatmanager.h>
#include <qutim/dataforms.h>
#include <qutim/config.h>

namespace qutim_sdk_0_3 {
namespace irc {

class IrcAccount;
class IrcChannel;

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
	Q_OBJECT
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
	void setAutoJoin(IrcChannel *channel, bool autojoin = true);
private:
	void saveBookmarkToConfig(Config &cfg, const IrcBookmark &bookmark);
	void addBookmark(const IrcBookmark &bookmark, const QString &oldName = QString());
	IrcBookmark loadBookmarkFromConfig(Config &cfg);
	QHash<QString, IrcBookmark> m_bookmarks;
	QList<IrcBookmark> m_recent;
};

} // namespace irc
} // namespace qutim_sdk_0_3

#endif // IRCGROUPCHATMANAGER_H


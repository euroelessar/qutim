#include "jbookmarkmanager.h"
#include "jmucmanager.h"
#include "../jaccount.h"
#include <gloox/bookmarkstorage.h>
#include <qutim/dataforms.h>

namespace Jabber
{
	struct JBookmarkManagerPrivate
	{
		JAccount *account;
		BookmarkStorage *storage;
		QList<JBookmark> bookmarks;
		QList<JBookmark> recent;
		bool isLoaded;
	};

	JBookmark::JBookmark(const QString &o_name, const QString &o_conference,
						 const QString &o_nick, const QString &o_password, bool o_autojoin)
	{
		name = o_name;
		conference = o_conference;
		nick = o_nick;
		password = o_password;
		autojoin =o_autojoin;		
	}

	bool JBookmark::operator==(const JBookmark &other) const
	{
		return (this->conference == other.conference) && (this->nick == other.nick);
	}

	bool JBookmark::isEmpty()
	{
		return name.isEmpty();
	}

	JBookmarkManager::JBookmarkManager(JAccount *account) : p(new JBookmarkManagerPrivate)
	{
		p->account = account;
		p->isLoaded = false;
		p->storage = new BookmarkStorage(account->client());
		p->storage->registerBookmarkHandler(this);
		p->bookmarks = readFromCache("bookmarks");
		p->recent = readFromCache("recent");
		emit bookmarksChanged();
	}

	JBookmarkManager::~JBookmarkManager()
	{

	}

	void JBookmarkManager::handleBookmarks(const BookmarkList &bList, const ConferenceList &cList)
	{
		QList<ConferenceListItem> confList = QList<ConferenceListItem>::fromStdList(cList);
		QList<JBookmark> tmpList(p->bookmarks);
		p->bookmarks.clear();
		foreach (ConferenceListItem conf, confList)
			p->bookmarks << JBookmark(QString::fromStdString(conf.name), QString::fromStdString(conf.jid),
									  QString::fromStdString(conf.nick), QString::fromStdString(conf.password), conf.autojoin);
		foreach (JBookmark bookmark, tmpList)
			if (p->bookmarks.contains(bookmark))
				p->bookmarks[p->bookmarks.indexOf(bookmark)].password = bookmark.password;
		writeToCache("bookmarks", p->bookmarks);
		Config config = p->account->config();
		int num = config.beginArray("urlmarks");
		foreach (const BookmarkListItem &item, bList) {
			Config configBookmarks = config.arrayElement(num);
			configBookmarks.setValue("name", QString::fromStdString(item.name));
			configBookmarks.setValue("url", QString::fromStdString(item.url));
			++num;
		}
		if (!p->isLoaded)
			foreach (JBookmark bookmark, p->bookmarks)
				if (bookmark.autojoin)
					p->account->conferenceManager()->join(bookmark.conference, bookmark.nick, bookmark.password);
		p->isLoaded = true;
		emit serverBookmarksChanged();
	}

	QList<JBookmark> JBookmarkManager::bookmarks() const
	{
		return p->bookmarks;
	}

	QList<JBookmark> JBookmarkManager::recent() const
	{
		return p->recent;
	}

	void JBookmarkManager::saveBookmark(int index, const QString &name, const QString &conference,
										const QString &nick, const QString &password, bool autojoin)
	{
		JBookmark bookmark(name, conference, nick, password, autojoin);
		if (index == p->bookmarks.count() || index == -1)
			p->bookmarks << bookmark;
		else
			p->bookmarks.replace(index, bookmark);
		writeToCache("bookmarks", p->bookmarks);
		saveToServer();
	}

	bool JBookmarkManager::saveBookmark(const qutim_sdk_0_3::DataItem &item, const QString &oldName)
	{
		QString name = item.subitem("name").data<QString>();
		QString conference = item.subitem("conference").data<QString>();
		QString nick = item.subitem("nickname").data<QString>();
		QString password = item.subitem("password").data<QString>();
		bool autojoin = item.subitem("autojoin").data<bool>();
		//some checks
		if (name.isEmpty())
			name = conference;
		if (conference.isEmpty())
			return false;
		if (name != oldName && !oldName.isEmpty())
			removeBookmark(indexOfBookmark(oldName));
		int index = indexOfBookmark(name);
		saveBookmark(index,name,conference,nick,password,autojoin);
		return true;
	}

	void JBookmarkManager::saveRecent(const QString &conference, const QString &nick, const QString &password)
	{
		bool isChanged = false;
		JBookmark bookmark("", conference, nick, password);
		if (p->recent.contains(bookmark)) {
			int index = p->recent.indexOf(bookmark);
			if (p->recent[index].password != password) {
				p->recent[index].password = password;
				isChanged = true;
			}
		} else {
			p->recent.insert(0, bookmark);
			p->recent = p->recent.mid(0, 10);
			isChanged = true;
		}
		if (isChanged)
			writeToCache("recent", p->recent);
	}

	void JBookmarkManager::removeBookmark(int index)
	{
		if (index == -1)
			return;
		p->bookmarks.removeAt(index);
		writeToCache("bookmarks", p->bookmarks);
		saveToServer();
	}

	void JBookmarkManager::sync()
	{
		p->storage->requestBookmarks();
	}

	QList<JBookmark> JBookmarkManager::readFromCache(const QString &type)
	{
		QList<JBookmark> list;
		Config config = p->account->config();
		int count = config.beginArray(type);
		for (int num = 0; num < count; num++) {
			Config configBookmark = config.arrayElement(num);
			JBookmark bookmark(configBookmark.value("name", QString()),
							   configBookmark.value("conference", QString()),
							   configBookmark.value("nick", QString()),
							   configBookmark.value("password", QString(), Config::Crypted),
							   configBookmark.value("autojoin", false));
			list << bookmark;
		}
		return list;
	}

	void JBookmarkManager::clearRecent()
	{
		Config config = p->account->config();
		config.remove(QString("recent"));
		config.sync();
	}

	void JBookmarkManager::writeToCache(const QString &type, const QList<JBookmark> &list)
	{
		Config config = p->account->config();
		config.remove(type);
		config.beginArray(type);
		for (int i = 0; i < list.size(); i++) {
			config.setArrayIndex(i);
			const JBookmark &bookmark = list.at(i);
			config.setValue("name", bookmark.name);
			config.setValue("conference", bookmark.conference);
			config.setValue("nick", bookmark.nick);
			config.setValue("password", bookmark.password, Config::Crypted);
			config.setValue("autojoin", bookmark.autojoin);
		}
		config.endArray();
		if (type == "bookmarks")
			emit bookmarksChanged();
	}

	void JBookmarkManager::saveToServer() {
		QList<ConferenceListItem> confList;
		foreach (JBookmark bookmark, p->bookmarks) {
			ConferenceListItem serverBookmark;
			serverBookmark.name = bookmark.name.toStdString();
			serverBookmark.jid = bookmark.conference.toStdString();
			serverBookmark.nick = bookmark.nick.toStdString();
			serverBookmark.autojoin = bookmark.autojoin;
			confList << serverBookmark;
		}
		QList<BookmarkListItem> urlList;
		Config urlmarks = p->account->config("urlmarks");
		int count = urlmarks.arraySize();
		for (int num = 0; num < count; num++) {
			Config configUrlmark = urlmarks.arrayElement(num);
			BookmarkListItem serverUrlmark;
			serverUrlmark.name = configUrlmark.value("name", QString()).toStdString();
			serverUrlmark.url = configUrlmark.value("url", QString()).toStdString();
			urlList << serverUrlmark;
		}
		p->storage->storeBookmarks(urlList.toStdList(), confList.toStdList());
	}

	JBookmark JBookmarkManager::find(const QString &name, bool recent) const
	{
		QList<JBookmark> bookmarks = recent ? p->recent : p->bookmarks;
		foreach (const JBookmark &item,bookmarks) {
			if (item.name == name)
				return item;
			else if (item.conference == name) //also looking for items that do not specify the name
				return item;
		}
		return JBookmark();
	}

	int JBookmarkManager::indexOfBookmark(const QString &name)
	{
		return p->bookmarks.indexOf(find(name,false));
	}
}

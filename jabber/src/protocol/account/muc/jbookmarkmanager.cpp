#include "jbookmarkmanager.h"
#include "jmucmanager.h"
#include "../jaccount.h"
#include <gloox/bookmarkstorage.h>

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

	QList<JBookmark> JBookmarkManager::bookmarks()
	{
		return p->bookmarks;
	}

	QList<JBookmark> JBookmarkManager::recent()
	{
		return p->recent;
	}

	void JBookmarkManager::saveBookmark(int index, const QString &name, const QString &conference,
			const QString &nick, const QString &password, bool autojoin)
	{
		JBookmark bookmark(name, conference, nick, password, autojoin);
		if (index == p->bookmarks.count())
			p->bookmarks << bookmark;
		else
			p->bookmarks.replace(index, bookmark);
		writeToCache("bookmarks", p->bookmarks);
		saveToServer();
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
		Config configBookmarks = p->account->config().group(type);
		int count = configBookmarks.arraySize();
		for (int num = 0; num < count; num++) {
			Config configBookmark = configBookmarks.arrayElement(num);
			JBookmark bookmark(configBookmark.value("name", QString()),
					configBookmark.value("conference", QString()),
					configBookmark.value("nick", QString()),
					configBookmark.value("password", QString(), Config::Crypted),
					configBookmark.value("autojoin", false));
			list << bookmark;
		}
		return list;
	}

	void JBookmarkManager::writeToCache(const QString &type, const QList<JBookmark> &list)
	{
		Config config = p->account->config();
		config.remove(type);
		int num = config.beginArray(type);
		foreach (const JBookmark &bookmark, list) {
			Config configBookmark = config.arrayElement(num);
			configBookmark.setValue("name", bookmark.name);
			configBookmark.setValue("conference", bookmark.conference);
			configBookmark.setValue("nick", bookmark.nick);
			configBookmark.setValue("password", bookmark.password, Config::Crypted);
			configBookmark.setValue("autojoin", bookmark.autojoin);
			++num;
		}
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
}

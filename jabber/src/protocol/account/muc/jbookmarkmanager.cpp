#include "jbookmarkmanager.h"
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
		readFromCache("bookmarks", p->bookmarks);
		readFromCache("recent", p->recent);
	}

	JBookmarkManager::~JBookmarkManager()
	{

	}

	void JBookmarkManager::handleBookmarks(const BookmarkList &bList, const ConferenceList &cList)
	{
		QList<ConferenceListItem> confList = QList<ConferenceListItem>::fromStdList(cList);
		p->bookmarks.clear();
		foreach (ConferenceListItem conf, confList)
			p->bookmarks << JBookmark(QString::fromStdString(conf.name), QString::fromStdString(conf.jid),
					QString::fromStdString(conf.nick), QString::fromStdString(conf.password), conf.autojoin);
		writeToCache("bookmark", p->bookmarks);
		QList<BookmarkListItem> urlList = QList<BookmarkListItem>::fromStdList(bList);
		for (int num = 0; num < urlList.count(); num++) {
			ConfigGroup configBookmarks = p->account->config().group("urlmarks").at(num);
			configBookmarks.setValue("name", QString::fromStdString(urlList[num].name));
			configBookmarks.setValue("url", QString::fromStdString(urlList[num].url));
			configBookmarks.sync();
		}
		/*if (!isLoaded)
			foreach (JBookmark bookmark, p->bookmarks)
				if (bookmark.autojoin)
					joinConference(conference, nick, password);*/
		emit bookmarksChanged();
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
		p->bookmarks.insert(index, bookmark);
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

	void JBookmarkManager::readFromCache(const QString &type, const QList<JBookmark> &list)
	{
		ConfigGroup configBookmarks = p->account->config(type);
		int count = configBookmarks.arraySize();
		for (int num = 0; num < count; num++) {
			ConfigGroup configBookmark = configBookmarks.at(num);
			JBookmark bookmark(configBookmark.value("name", QString()),
					configBookmark.value("conference", QString()),
					configBookmark.value("nick", QString()),
					configBookmark.value("password", QString(), Config::Crypted),
					configBookmark.value("autojoin", false));
			p->bookmarks << bookmark;
		}
	}

	void JBookmarkManager::writeToCache(const QString &type, const QList<JBookmark> &list)
	{
		p->account->config().removeGroup(type);
		for (int num = 0; num < list.count(); num++) {
			ConfigGroup configBookmarks = p->account->config().group(type).at(num);
			configBookmarks.setValue("name", list[num].name);
			configBookmarks.setValue("conference", list[num].conference);
			configBookmarks.setValue("nick", list[num].nick);
			configBookmarks.setValue("password", list[num].password, Config::Crypted);
			configBookmarks.setValue("autojoin", list[num].autojoin);
			configBookmarks.sync();
		}
	}

	void JBookmarkManager::saveToServer() {
		QList<ConferenceListItem> confList;
		foreach (JBookmark bookmark, p->bookmarks) {
			ConferenceListItem serverBookmark;
			serverBookmark.name = bookmark.name.toStdString();
			serverBookmark.jid = bookmark.conference.toStdString();
			serverBookmark.nick = bookmark.nick.toStdString();
			serverBookmark.password = bookmark.password.toStdString();
			serverBookmark.autojoin = bookmark.autojoin;
			confList << serverBookmark;
		}
		QList<BookmarkListItem> urlList;
		ConfigGroup urlmarks = p->account->config("urlmarks");
		int count = urlmarks.arraySize();
		for (int num = 0; num < count; num++) {
			ConfigGroup configUrlmark = urlmarks.at(num);
			BookmarkListItem serverUrlmark;
			serverUrlmark.name = configUrlmark.value("name", QString()).toStdString();
			serverUrlmark.url = configUrlmark.value("url", QString()).toStdString();
			urlList << serverUrlmark;
		}
		p->storage->storeBookmarks(urlList.toStdList(), confList.toStdList());
	}
}

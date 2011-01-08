#include "jbookmarkmanager.h"
#include "jmucmanager.h"
#include "../jaccount.h"
#include <qutim/dataforms.h>
#include <qutim/notificationslayer.h>
#include <qutim/debug.h>

namespace Jabber
{
using namespace jreen;
struct JBookmarkManagerPrivate
{
	JAccount *account;
	BookmarkStorage *storage;
	QList<Bookmark::Conference> bookmarks;
	QList<Bookmark::Conference> recent;
	bool isLoaded;
};

JBookmarkManager::JBookmarkManager(JAccount *account) :
	GroupChatManager(account),
	p(new JBookmarkManagerPrivate)
{
	p->account = account;
	p->isLoaded = false;
	p->storage = new BookmarkStorage(account->client());
	p->storage->setPrivateXml(account->privateXml());
	p->storage->setPubSubManager(account->pubSubManager());
	connect(p->storage, SIGNAL(bookmarksReceived(jreen::Bookmark::Ptr)),
			this, SLOT(onBookmarksReceived(jreen::Bookmark::Ptr)));
	//		p->storage->registerBookmarkHandler(this);
	p->bookmarks = readFromCache("bookmarks");
	p->recent = readFromCache("recent");
	emit bookmarksChanged();
}

JBookmarkManager::~JBookmarkManager()
{

}

void JBookmarkManager::onBookmarksReceived(const jreen::Bookmark::Ptr &bookmark)
{
	qDebug("%s %d", Q_FUNC_INFO, bookmark->conferences().size());
	QList<Bookmark::Conference> tmpList(p->bookmarks);
	p->bookmarks = bookmark->conferences();
	foreach (const Bookmark::Conference &bookmark, tmpList) {
		Bookmark::Conference current = find(bookmark.name());
		current.setPassword(bookmark.password());
	}
	writeToCache("bookmarks", p->bookmarks);
	//		Config config = p->account->config();
	//		int num = config.beginArray("urlmarks");
	//		foreach (const Bookmark::Conference &item, bList) {
	//			Config configBookmarks = config.arrayElement(num);
	//			configBookmarks.setValue("name", QString::fromStdString(item.name));
	//			configBookmarks.setValue("url", QString::fromStdString(item.url));
	//			++num;
	//		}
	if (!p->isLoaded) {
		foreach (Bookmark::Conference bookmark, p->bookmarks)
			if (bookmark.autojoin())
				p->account->conferenceManager()->join(bookmark.jid(), bookmark.nick(), bookmark.password());
	}
	p->isLoaded = true;
	emit serverBookmarksChanged();
}

QList<Bookmark::Conference> JBookmarkManager::bookmarksList() const
{
	return p->bookmarks;
}

QList<Bookmark::Conference> JBookmarkManager::recentList() const
{
	return p->recent;
}

void JBookmarkManager::saveBookmark(int index, const QString &name, const QString &conference,
									const QString &nick, const QString &password, bool autojoin)
{
	Bookmark::Conference bookmark(name, conference, nick, password, autojoin);
	if (index == p->bookmarks.count() || index == -1)
		p->bookmarks << bookmark;
	else
		p->bookmarks.replace(index, bookmark);
	writeToCache("bookmarks", p->bookmarks);
	saveToServer();
}

void JBookmarkManager::saveRecent(const QString &conference, const QString &nick, const QString &password)
{
	Bookmark::Conference bookmark(QString(), conference, nick, password);
	Bookmark::Conference tmp = find(conference, true);
	if (tmp.isValid() && tmp.nick() != nick) {
		if (tmp.password() != password)
			tmp.setPassword(bookmark.password());
		else
			return;
	} else {
		p->recent.prepend(bookmark);
		while (p->recent.size() > 10)
			p->recent.removeLast();
	}
	writeToCache("recent", p->recent);
}

bool JBookmarkManager::removeBookmark(const jreen::Bookmark::Conference &bookmark)
{
	if (p->bookmarks.removeOne(bookmark)) {
		writeToCache("bookmarks", p->bookmarks);
		saveToServer();
		return true;
	}
	return false;
}

void JBookmarkManager::sync()
{
	p->storage->requestBookmarks();
}

QList<Bookmark::Conference> JBookmarkManager::readFromCache(const QString &type)
{
	QList<Bookmark::Conference> list;
	Config config = p->account->config();
	int count = config.beginArray(type);
	for (int num = 0; num < count; num++) {
		Config configBookmark = config.arrayElement(num);
		Bookmark::Conference bookmark;
		bookmark.setName(configBookmark.value("name", QString()));
		bookmark.setJid(configBookmark.value("conference", QString()));
		bookmark.setNick(configBookmark.value("nick", QString()));
		bookmark.setPassword(configBookmark.value("password", QString(), Config::Crypted));
		bookmark.setAutojoin(configBookmark.value("autojoin", false));
		list << bookmark;
	}
	return list;
}

void JBookmarkManager::clearRecent()
{
	Config config = p->account->config();
	config.remove(QLatin1String("recent"));
	config.sync();
}

DataItem JBookmarkManager::fields(const Bookmark::Conference &bookmark, bool isBookmark) const
{
	qutim_sdk_0_3::DataItem item(bookmark.name().isEmpty() ? bookmark.jid().bare() : bookmark.name());
	if (bookmark.isValid()) {
		item.setProperty("bookmark", qVariantFromValue(bookmark));
	}
	{
		qutim_sdk_0_3::DataItem nameItem("name", QT_TRANSLATE_NOOP("Jabber", "Name"), bookmark.name());
		if (!isBookmark)
			nameItem.setProperty("showInBookmarkInfo", false);
		item.addSubitem(nameItem);
	}
	{
		QString conference = !bookmark.jid().isValid() ? QString("talks@conference.qutim.org") : bookmark.jid().bare();
		qutim_sdk_0_3::DataItem conferenceItem("conference", QT_TRANSLATE_NOOP("Jabber", "Conference"), conference);
		//TODO, add validator
		//conferenceItem.setProperty("validator", QRegExp("^(#|&|!|\\+)[^\\s0x0007,]{1,50}"));
		conferenceItem.setProperty("mandatory", true);
		item.addSubitem(conferenceItem);
	}
	{
		QString name = bookmark.nick().isEmpty() ? p->account->name() : bookmark.nick();
		qutim_sdk_0_3::DataItem nickItem("nickname", QT_TRANSLATE_NOOP("Jabber", "Nick"), name);
		nickItem.setProperty("mandatory", true);
		item.addSubitem(nickItem);
	}
	{
		qutim_sdk_0_3::DataItem passwordItem("password", QT_TRANSLATE_NOOP("Jabber", "Password"), bookmark.password());
		passwordItem.setProperty("password", true);
		passwordItem.setProperty("showInBookmarkInfo", false);
		item.addSubitem(passwordItem);
	}
	{
		qutim_sdk_0_3::DataItem autoJoinItem("autojoin",QT_TRANSLATE_NOOP("Jabber", "Auto-join"),QVariant(bookmark.autojoin()));
		if (!isBookmark)
			autoJoinItem.setProperty("showInBookmarkInfo", false);
		item.addSubitem(autoJoinItem);
	}
	return item;
}

DataItem JBookmarkManager::fields() const
{
	return fields(Bookmark::Conference());
}

bool JBookmarkManager::join(const DataItem &fields)
{
	JID conference = fields.subitem("conference").data<QString>();
	QString nickname = fields.subitem("nickname").data<QString>();
	if (!conference.isBare() || nickname.isEmpty()) {
		Notifications::send(tr("Unable to join groupchat"));
		return false;
	}
	QString password = fields.subitem("password").data<QString>();
	p->account->conferenceManager()->join(conference, nickname, password);
	return true;
}

bool JBookmarkManager::storeBookmark(const DataItem &fields, const DataItem &oldFields)
{
	Bookmark::Conference conf = oldFields.property("bookmark", Bookmark::Conference());
	bool isValid = conf.isValid();
	JID jid = fields.subitem("conference").data<QString>();
	QString nick = fields.subitem("nickname").data<QString>();
	if (!jid.isBare() || nick.isEmpty())
		return false;
	conf.setName(fields.subitem("name").data<QString>());
	if (conf.name().isEmpty())
		conf.setName(jid.bare());
	conf.setJid(jid);
	conf.setNick(nick);
	conf.setPassword(fields.subitem("password").data<QString>());
	conf.setAutojoin(fields.subitem("autojoin").data<bool>());
	if (!isValid)
		p->bookmarks.append(conf);
	writeToCache("bookmarks", p->bookmarks);
	saveToServer();
	return true;
}

bool JBookmarkManager::removeBookmark(const DataItem &fields)
{
	return removeBookmark(fields.property("bookmark").value<Bookmark::Conference>());
}

QList<DataItem> JBookmarkManager::bookmarks() const
{
	QList<DataItem> bookmarks;
	foreach (const Bookmark::Conference &bookmark, p->bookmarks) {
		bookmarks << fields(bookmark);
	}
	return bookmarks;
}

QList<DataItem> JBookmarkManager::recent() const
{
	QList<DataItem> bookmarks;
	foreach (const Bookmark::Conference &bookmark, p->recent)
		bookmarks << fields(bookmark, false);
	return bookmarks;
}

void JBookmarkManager::writeToCache(const QString &type, const QList<Bookmark::Conference> &list)
{
	Config config = p->account->config();
	config.remove(type);
	config.beginArray(type);
	for (int i = 0; i < list.size(); i++) {
		config.setArrayIndex(i);
		const Bookmark::Conference &bookmark = list.at(i);
		config.setValue("name", bookmark.name());
		config.setValue("conference", bookmark.jid().full());
		config.setValue("nick", bookmark.nick());
		config.setValue("password", bookmark.password(), Config::Crypted);
		config.setValue("autojoin", bookmark.autojoin());
	}
	config.endArray();
	if (type == "bookmarks")
		emit bookmarksChanged();
}

void JBookmarkManager::saveToServer() {
	Bookmark::Ptr bookmark = Bookmark::Ptr::create();
	bookmark->setConferences(p->bookmarks);
	p->storage->storeBookmarks(bookmark);
	//		QList<BookmarkListItem> urlList;
	//		Config urlmarks = p->account->config("urlmarks");
	//		int count = urlmarks.arraySize();
	//		for (int num = 0; num < count; num++) {
	//			Config configUrlmark = urlmarks.arrayElement(num);
	//			BookmarkListItem serverUrlmark;
	//			serverUrlmark.name = configUrlmark.value("name", QString()).toStdString();
	//			serverUrlmark.url = configUrlmark.value("url", QString()).toStdString();
	//			urlList << serverUrlmark;
	//		}
	//		p->storage->storeBookmarks(urlList.toStdList(), confList.toStdList());
}

Bookmark::Conference JBookmarkManager::find(const QString &name, bool recent) const
{
	const QList<Bookmark::Conference> &bookmarks = recent ? p->recent : p->bookmarks;
	foreach (const Bookmark::Conference &item, bookmarks) {
		if (item.name() == name || item.jid() == name) //also looking for items that do not specify the name
			return item;
	}
	return Bookmark::Conference();
}

int JBookmarkManager::indexOfBookmark(const QString &name) const
{
	for (int i = 0; i < p->bookmarks.size(); i++) {
		const Bookmark::Conference &item = p->bookmarks.at(i);
		if (item.name() == name || item.jid() == name)
			return i;
	}
	return -1;
}
}

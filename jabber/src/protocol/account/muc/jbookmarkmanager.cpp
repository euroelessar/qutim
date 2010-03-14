#include "jbookmarkmanager.h"
#include "../jaccount.h"
#include <gloox/bookmarkstorage.h>

namespace Jabber
{
	struct JBookmarkManagerPrivate
	{
		BookmarkStorage *storage;
	};

	JBookmarkManager::JBookmarkManager(JAccount *account) : p(new JBookmarkManagerPrivate)
	{
		p->storage = new BookmarkStorage(account->client());
		p->storage->registerBookmarkHandler(this);
	}

	JBookmarkManager::~JBookmarkManager()
	{

	}

	void JBookmarkManager::handleBookmarks(const BookmarkList &bList, const ConferenceList &cList)
	{
		emit bookmarksChanged();
	}

	QList<JBookmark> JBookmarkManager::bookmarks()
	{
		QList<JBookmark> b;
		return b;
	}

	void JBookmarkManager::saveBookmark(JBookmark bookmark, const QString &name, const QString &conference,
			const QString &nick, const QString &password)
	{

	}

	void JBookmarkManager::saveBookmark(const QString &name, const QString &conference, const QString &nick, const QString &password)
	{

	}

	void JBookmarkManager::saveRecent(const QString &conference, const QString &nick, const QString &password)
	{

	}

	void JBookmarkManager::removeBookmark(JBookmark bookmark)
	{

	}

	void JBookmarkManager::sync()
	{
		p->storage->requestBookmarks();
	}
}

#ifndef JBOOKMARKMANAGER_H
#define JBOOKMARKMANAGER_H

#include <qutim/libqutim_global.h>
#include <gloox/bookmarkhandler.h>

namespace Jabber {

	using namespace gloox;

	struct JBookmarkManagerPrivate;
	class JAccount;

	struct JBookmark
	{
		QString name;
		QString conference;
		QString nick;
		QString password;
		bool autojoin;
	};

	class JBookmarkManager : public QObject, public BookmarkHandler
	{
		Q_OBJECT
		public:
			JBookmarkManager(JAccount *account);
			~JBookmarkManager();
			void handleBookmarks(const BookmarkList &bList, const ConferenceList &cList);
			QList<JBookmark> bookmarks();
			void saveBookmark(JBookmark bookmark, const QString &name, const QString &conference,
					const QString &nick, const QString &password);
			void saveBookmark(const QString &name, const QString &conference, const QString &nick, const QString &password);
			void saveRecent(const QString &conference, const QString &nick, const QString &password);
			void removeBookmark(JBookmark bookmark);
			void sync();
		signals:
			void bookmarksChanged();
		private:
			QScopedPointer<JBookmarkManagerPrivate> p;
	};
}

Q_DECLARE_METATYPE(Jabber::JBookmark)

#endif // JBOOKMARKMANAGER_H

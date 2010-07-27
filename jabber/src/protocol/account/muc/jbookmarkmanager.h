#ifndef JBOOKMARKMANAGER_H
#define JBOOKMARKMANAGER_H

#include <qutim/libqutim_global.h>
#include <gloox/bookmarkhandler.h>
#include <QMetaType>

namespace Jabber {

	using namespace gloox;

	struct JBookmarkManagerPrivate;
	class JAccount;

	struct JBookmark
	{
		JBookmark(const QString &o_name, const QString &o_conference,
					 const QString &o_nick, const QString &o_password, bool o_autojoin = false);
		inline JBookmark() {}
		bool operator==(const JBookmark &other) const;
		bool isEmpty();
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
			QList<JBookmark> bookmarks() const;
			QList<JBookmark> recent() const;
			JBookmark find(const QString &name, bool recent = false) const;
			void saveBookmark(int index, const QString &name, const QString &conference,
					const QString &nick, const QString &password, bool autojoin = false);
			void saveRecent(const QString &conference, const QString &nick, const QString &password);
			void removeBookmark(int index);
			void sync();

		signals:
			void serverBookmarksChanged();
			void bookmarksChanged();
		protected:
			void handleBookmarks(const BookmarkList &bList, const ConferenceList &cList);
			QList<JBookmark> readFromCache(const QString &type);
			void writeToCache(const QString &type, const QList<JBookmark> &list);
			void saveToServer();
		private:
			QScopedPointer<JBookmarkManagerPrivate> p;
	};
}

Q_DECLARE_METATYPE(Jabber::JBookmark)

#endif // JBOOKMARKMANAGER_H

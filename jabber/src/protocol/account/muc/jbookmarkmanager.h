#ifndef JBOOKMARKMANAGER_H
#define JBOOKMARKMANAGER_H

#include <qutim/libqutim_global.h>
#include <qutim/groupchatmanager.h>
#include <gloox/bookmarkhandler.h>
#include <QMetaType>

namespace qutim_sdk_0_3
{
	class DataItem;
}

namespace Jabber {

	using qutim_sdk_0_3::DataItem;

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

	class JBookmarkManager : public QObject, public gloox::BookmarkHandler, public qutim_sdk_0_3::GroupChatManager
	{
		Q_OBJECT
		Q_INTERFACES(qutim_sdk_0_3::GroupChatManager)
		public:
			JBookmarkManager(JAccount *account);
			~JBookmarkManager();
			QList<JBookmark> bookmarksList() const;
			QList<JBookmark> recentList() const;
			JBookmark find(const QString &name, bool recent = false) const;
			int indexOfBookmark(const QString &name);
			void saveBookmark(int index, const QString &name, const QString &conference,
					const QString &nick, const QString &password, bool autojoin = false);
			void saveRecent(const QString &conference, const QString &nick, const QString &password);
			void removeBookmark(int index);
			void sync();
			void clearRecent();
			DataItem fields(const JBookmark &bookmark, bool isBookmark = true) const;

			// Group chat manager
			DataItem fields() const;
			bool join(const DataItem &fields);
			bool storeBookmark(const DataItem &fields, const DataItem &oldFields);
			bool removeBookmark(const DataItem &fields);
			QList<DataItem> bookmarks() const;
			QList<DataItem> recent() const;
		signals:
			void serverBookmarksChanged();
			void bookmarksChanged();
		protected:
			void handleBookmarks(const gloox::BookmarkList &bList, const gloox::ConferenceList &cList);
			QList<JBookmark> readFromCache(const QString &type);
			void writeToCache(const QString &type, const QList<JBookmark> &list);
			void saveToServer();
		private:
			QScopedPointer<JBookmarkManagerPrivate> p;
	};
}

Q_DECLARE_METATYPE(Jabber::JBookmark)

#endif // JBOOKMARKMANAGER_H

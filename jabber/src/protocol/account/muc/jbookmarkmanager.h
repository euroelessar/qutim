#ifndef JBOOKMARKMANAGER_H
#define JBOOKMARKMANAGER_H

#include <qutim/libqutim_global.h>
#include <qutim/groupchatmanager.h>
#include <jreen/bookmarkstorage.h>
#include <QMetaType>

namespace qutim_sdk_0_3
{
class DataItem;
}

namespace Jabber {

using qutim_sdk_0_3::DataItem;

struct JBookmarkManagerPrivate;
class JAccount;

class JBookmarkManager : public QObject, public qutim_sdk_0_3::GroupChatManager
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::GroupChatManager)
public:
	JBookmarkManager(JAccount *account);
	~JBookmarkManager();
	QList<Jreen::Bookmark::Conference> bookmarksList() const;
	QList<Jreen::Bookmark::Conference> recentList() const;
	Jreen::Bookmark::Conference find(const QString &name, bool recent = false) const;
	int find(const Jreen::Bookmark::Conference &bookmark, bool recent = false) const;
	//			Jreen::Bookmark::Conference find(const QString &name, const QString &nick, bool recent = false) const;
	int indexOfBookmark(const QString &name) const;
	void saveBookmark(int index, const QString &name, const QString &conference,
					  const QString &nick, const QString &password, bool autojoin = false);
	void saveRecent(const QString &conference, const QString &nick, const QString &password);
	bool removeBookmark(const Jreen::Bookmark::Conference &bookmark);
	void sync();
	void clearRecent();
	DataItem fields(const Jreen::Bookmark::Conference &bookmark, bool isBookmark = true) const;

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
protected slots:
	void onBookmarksReceived(const Jreen::Bookmark::Ptr &bookmark);
protected:
	QList<Jreen::Bookmark::Conference> readFromCache(const QString &type);
	void writeToCache(const QString &type, const QList<Jreen::Bookmark::Conference> &list);
	void saveToServer();
private:
	QScopedPointer<JBookmarkManagerPrivate> p;
};
}

Q_DECLARE_METATYPE(Jreen::Bookmark::Conference)

#endif // JBOOKMARKMANAGER_H

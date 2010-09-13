#ifndef BOOKMARKSMODEL_H
#define BOOKMARKSMODEL_H

#include <QAbstractListModel>

namespace Core {

	enum BookmarkType
	{
		BookmarkNew,
		BookmarkEdit,
		BookmarkItem,
		BookmarkEmptyItem,
		BookmarkRecentItem,
		BookmarkSeparator
	};

	enum BookmarkRoles
	{
		BookmarkTypeRole = Qt::UserRole
	};

	class BookmarksModel : public QAbstractListModel
	{
	public:
		BookmarksModel(QObject *parent = 0);
		int rowCount(const QModelIndex &parent = QModelIndex()) const;
		Qt::ItemFlags flags(const QModelIndex &index) const;
		QVariant data(int index, int role = Qt::DisplayRole) const;
		QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
		void addItem(BookmarkType type, const QString &text, const QVariant &fields = QVariantMap());
		void startUpdating();
		void endUpdating();
		void clear();
	private:
		struct Bookmark
		{
			Bookmark() {}
			Bookmark(BookmarkType type_, const QString &text_, const QVariant &fields_) :
				type(type_), text(text_), fields(fields_)
			{}
			BookmarkType type;
			QString text;
			QVariant fields;
		};
		QList<Bookmark> m_bookmarks;
		bool m_resetting;
	};

} // namespace Core

Q_DECLARE_METATYPE(Core::BookmarkType)

#endif // BOOKMARKSMODEL_H

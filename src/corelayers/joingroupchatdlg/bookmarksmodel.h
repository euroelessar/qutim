/****************************************************************************
 *  bookmarksmodel.h
 *
 *  Copyright (c) 2010 by Alexey Prokhin <alexey.prokhin@yandex.ru>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

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
	BookmarkTypeRole = Qt::UserRole + 1
};

class BookmarksModel : public QAbstractListModel
{
public:
	BookmarksModel(QObject *parent = 0);
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	QVariant data(int index, int role = Qt::DisplayRole) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	void addItem(BookmarkType type, const QString &text,
				 const QVariant &fields = QVariantMap(),
				 const QVariant &userData = QVariant());
	void startUpdating();
	void endUpdating();
	void clear();
private:
	struct Bookmark
	{
		Bookmark() {}
		Bookmark(BookmarkType type_, const QString &text_, const QVariant &fields_, const QVariant &userData_) :
			type(type_), text(text_), fields(fields_), userData(userData_)
		{}
		BookmarkType type;
		QString text;
		QVariant fields;
		QVariant userData;
	};
	QList<Bookmark> m_bookmarks;
	bool m_resetting;
};

} // namespace Core

Q_DECLARE_METATYPE(Core::BookmarkType)

#endif // BOOKMARKSMODEL_H

/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#include "bookmarksmodel.h"
#include <qutim/itemdelegate.h>
#include <qutim/icon.h>

namespace Core {

using namespace qutim_sdk_0_3;

typedef QHash<BookmarkType, QIcon> Icons;
Icons init_icons()
{
    Icons icons;
	icons.insert(BookmarkNew, Icon("meeting-attending"));
	icons.insert(BookmarkEdit, Icon("bookmark-new-list"));
	icons.insert(BookmarkItem, Icon("bookmarks"));
	icons.insert(BookmarkRecentItem, Icon("view-history"));
    return icons;
}

Q_GLOBAL_STATIC_WITH_ARGS(Icons, icons, (init_icons()))

BookmarksModel::BookmarksModel(QObject *parent) :
	QAbstractListModel(parent), m_resetting(false)
{
}

int BookmarksModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return m_bookmarks.size();
}

Qt::ItemFlags BookmarksModel::flags(const QModelIndex &index) const
{
	Qt::ItemFlags flags = QAbstractListModel::flags(index);
	Bookmark bookmark = m_bookmarks.value(index.row());
	if (bookmark.type == BookmarkSeparator)
		flags &= ~(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	return flags;
}

QVariant BookmarksModel::data(int i, int role) const
{
	return data(index(i, 0), role);
}

QVariant BookmarksModel::data(const QModelIndex &index, int role) const
{
	Bookmark bookmark = m_bookmarks.value(index.row());
	switch (role) {
	case Qt::DisplayRole:
		return bookmark.text;
	case Qt::DecorationRole:
		return icons()->value(bookmark.type);
	case DescriptionRole:
		return bookmark.fields;
	case BookmarkTypeRole:
		return QVariant::fromValue(bookmark.type);
	case Qt::AccessibleDescriptionRole:
		if (bookmark.type == BookmarkSeparator)
			return "separator";
	case SeparatorRole:
		if (bookmark.type == BookmarkSeparator)
			return true;
	case Qt::UserRole:
		return bookmark.userData;
	}
	return QVariant();
}

void BookmarksModel::addItem(BookmarkType type, const QString &text,
							 const QVariant &fields, const QVariant &userData)
{
	if (!m_resetting) {
		int pos = m_bookmarks.size();
		beginInsertRows(QModelIndex(), pos, pos);
	}
	m_bookmarks.push_back(Bookmark(type, text, fields, userData));
	if (!m_resetting)
		endRemoveRows();
}

void BookmarksModel::startUpdating()
{
	beginResetModel();
	m_bookmarks.clear();
	m_resetting = true;
}

void BookmarksModel::endUpdating()
{
	endResetModel();
	m_resetting = false;
}

void BookmarksModel::clear()
{
	beginResetModel();
	m_bookmarks.clear();
	endResetModel();
}

} // namespace Core


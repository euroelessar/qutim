/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
** Copyright © 2011 Evgeniy Degtyarev <degtep@gmail.com>
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
#include <qutim/groupchatmanager.h>
#include <qutim/protocol.h>

namespace MeegoIntegration {
const static int m_max_recent_count = 4;
using namespace qutim_sdk_0_3;

typedef QHash<BookmarkType, QIcon> Icons;
void init_icons(Icons &icons)
{
	icons.insert(BookmarkNew, Icon("meeting-attending"));
	icons.insert(BookmarkEdit, Icon("bookmark-new-list"));
	icons.insert(BookmarkItem, Icon("bookmarks"));
	icons.insert(BookmarkRecentItem, Icon("view-history"));
}

Q_GLOBAL_STATIC_WITH_INITIALIZER(Icons, icons, init_icons(*x));


BookmarksModel::BookmarksModel(QObject *parent) :
	QAbstractListModel(parent)
{
	QHash<int, QByteArray> roleNames;
	roleNames.insert(BookmarkDataRole, "bookmarkData");
	roleNames.insert(Qt::DisplayRole, "name");
	roleNames.insert(DescriptionRole, "description");
	roleNames.insert(BookmarkTypeRole, "bookmarkType");
	roleNames.insert(SeparatorRole, "separator");

	setRoleNames(roleNames);
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
		return bookmark.fields.value<QString>();
	case BookmarkTypeRole:
		return bookmark.type;
	case Qt::AccessibleDescriptionRole:
		if (bookmark.type == BookmarkSeparator)
			return "separator";
	case SeparatorRole:
		if (bookmark.type == BookmarkSeparator)
			return true;
		return false;
	case BookmarkDataRole:
		return bookmark.userData;
	}
	return QVariant();
}

void BookmarksModel::addItem(BookmarkType type, const QString &text,
			     const QVariant &fields, const QVariant &userData)
{
		int pos = m_bookmarks.size();
		beginInsertRows(QModelIndex(), pos, pos);
	m_bookmarks.append(Bookmark(type, text, fields, userData));
		endInsertRows();
}

void BookmarksModel::startUpdating()
{
	if (m_bookmarks.size()>0)
	{
	beginRemoveRows(QModelIndex(), 0, m_bookmarks.size());
	m_bookmarks.clear();
	endRemoveRows();
	}

}

void BookmarksModel::endUpdating()
{

}

void BookmarksModel::clear()
{
	if (m_bookmarks.size()>0)
	{
	beginRemoveRows(QModelIndex(), 0, m_bookmarks.size());
	m_bookmarks.clear();
	endRemoveRows();
	}
}

void BookmarksModel::fillBookmarks(const QList<DataItem> &bookmarks, bool recent)
{
	QString txt = recent ?
				QT_TRANSLATE_NOOP("JoinGroupChat", "Recent") :
				QT_TRANSLATE_NOOP("JoinGroupChat", "Bookmarks");
	addItem(BookmarkSeparator, txt);
	qDebug()<<"fillBookmarks";
	int count = 0;
	foreach (const DataItem &bookmark, bookmarks) {
		QString name = bookmark.title();
		QVariantMap fields;
		foreach (const DataItem &subitem, bookmark.subitems()) {
			if (subitem.property("showInBookmarkInfo", true))
				fields.insert(subitem.title(), subitem.data());
		}
		BookmarkType type = recent ? BookmarkRecentItem : BookmarkItem;
		QVariant userData = QVariant::fromValue(bookmark);

		addItem(type, name, fields, userData);
		++count;
		if (recent && (count >= m_max_recent_count))
			return;
	}
	qDebug()<<count;
}

void BookmarksModel::fill(QString accountId)
{
	foreach (Protocol *protocol, Protocol::all()) {
		foreach (Account *account, protocol->accounts())
		{
			if (account->id() == accountId )
			{

				qDebug()<<"fill(QString accountId)";
				GroupChatManager *manager = account->groupChatManager();
				if (!manager)
					return;

				startUpdating();
//TODO: need to implement
//				QVariant fields = qVariantFromValue(QT_TRANSLATE_NOOP("JoinGroupChat", "Join an existing or create a new groupchat"));
//				addItem(BookmarkNew,
//					QT_TRANSLATE_NOOP("JoinGroupChat", "Join"),
//					fields);
//				fields = qVariantFromValue(QT_TRANSLATE_NOOP("JoinGroupChat", "Create, edit, or delete saved bookmarks"));
//				addItem(BookmarkEdit,
//					QT_TRANSLATE_NOOP("JoinGroupChat", "Manage bookmarks"),
//					fields);

//				addItem(BookmarkEmptyItem,QString());
				//Bookmarks
				fillBookmarks(manager->bookmarks());
				//Recent items
				fillBookmarks(manager->recent(), true);
				endUpdating();
			}

		}}

}

} // namespace MeegoIntegration


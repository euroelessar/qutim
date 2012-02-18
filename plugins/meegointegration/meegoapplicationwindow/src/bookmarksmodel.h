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

#ifndef BOOKMARKSMODEL_H
#define BOOKMARKSMODEL_H

#include <QAbstractListModel>
#include <qutim/account.h>
#include <qutim/dataforms.h>

namespace MeegoIntegration {
using namespace qutim_sdk_0_3;
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
	BookmarkTypeRole = Qt::UserRole,
	BookmarkDataRole
};

class BookmarksModel : public QAbstractListModel
{
	Q_OBJECT
	Q_ENUMS(BookmarkType)
public:
	explicit BookmarksModel(QObject *parent = 0);
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	virtual QVariant data(int index, int role = Qt::DisplayRole) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	void addItem(BookmarkType type, const QString &text,
				 const QVariant &fields = QVariantMap(),
				 const QVariant &userData = QVariant());
	void startUpdating();
	void endUpdating();
	void clear();
	Q_INVOKABLE void fill(QString accountId);
private:
	void fillBookmarks(const QList<DataItem> &bookmarks, bool recent = false);
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
};

} // namespace MeegoIntegration

Q_DECLARE_METATYPE(MeegoIntegration::BookmarkType)

#endif // BOOKMARKSMODEL_H


/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#ifndef SIMPLECONTACTLISTVIEW_H
#define SIMPLECONTACTLISTVIEW_H

#include <QTreeView>
#include <QPersistentModelIndex>
#include "simplecontactlist_global.h"

namespace Core
{
namespace SimpleContactList
{

class SIMPLECONTACTLIST_EXPORT TreeView : public QTreeView
{
	Q_OBJECT
public:
	TreeView(QAbstractItemModel *model, QWidget *parent = 0);
    virtual void dataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight, const QVector<int> &roles = QVector<int>());
	virtual ~TreeView();
	void setContactModel(QAbstractItemModel *model);
protected:
	void contextMenuEvent(QContextMenuEvent *);
	void startDrag(Qt::DropActions supportedActions);
protected slots:
	void onClick(const QModelIndex &index);
	void initScrolling();
	void onCollapsed(const QModelIndex &index);
	void onExpanded(const QModelIndex &index);
	void onRowsInserted(const QModelIndex &parent, int first, int last);
private:
	void storeClosedTags();
	void checkTag(const QModelIndex &parent, QAbstractItemModel *model);
	QSet<QString> m_closedIndexes;
};

}
}

#endif // SIMPLECONTACTLISTVIEW_H


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

#ifndef PLUGPACKAGEITEM_H
#define PLUGPACKAGEITEM_H
#include "plugpackage.h"
#include <QVector>
#include <QHash>

struct downloaderItem;
class plugPackageItem : public QObject
{
    Q_OBJECT
public:
	plugPackageItem(ItemData* data);
	plugPackageItem();
	~plugPackageItem();
	void appendChild(plugPackageItem* data);
	void removeChild(const int& row);
	plugPackageItem *Child(const int &row);
	ItemData *getItemData ();
	int childCount();
	int indexOf(plugPackageItem *item);
	int row () const;
	void setItem (ItemData* data);
	plugPackageItem *getParent ();
private:
	ItemData *item_data;
	plugPackageItem *parent;
	QList<plugPackageItem *> childItems;
public slots:
	void iconDownloaded (const QList< downloaderItem >& items);
};

#endif // PLUGPACKAGEITEM_H


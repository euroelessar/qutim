/*
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef PLUGPACKAGEITEM_H
#define PLUGPACKAGEITEM_H
#include "plugpackage.h"
#include <QVector>
#include <QHash>

class plugPackageItem
{
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
};

#endif // PLUGPACKAGEITEM_H

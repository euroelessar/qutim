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

//переписываю по простому, потом буду усложнять/ускорять и тд
class plugPackageItem
{
public:
	plugPackageItem(const ItemData& data);
	plugPackageItem();
	~plugPackageItem();
	void addChild(plugPackageItem* data, const int& id);
	void removeChild(const int& id);
	inline plugPackageItem *getChildData(const int &number) {return children.value(number, 0);};
	inline ItemData *getItemData () {return &item_data;};
	inline int childrenCount() {return children.count();};
	inline int indexOf(plugPackageItem *item) {return children.key(item, 0);};
	void setItem (const ItemData& data);
	plugPackageItem *getParent () {return parent;};
	QString item_name;
private:
	ItemData item_data;
	plugPackageItem *parent;
	QHash<int, plugPackageItem *> children;
};

#endif // PLUGPACKAGEITEM_H

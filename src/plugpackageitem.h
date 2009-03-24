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
	plugPackageItem();
	plugPackageItem(packageInfo *item);
	virtual ~plugPackageItem();
	inline packageInfo *getItem() { return m_package_info; }
	void setData(packageInfo *item) { m_package_info = item; }
	inline plugPackageItem *getChild(int num) { return m_children_temp.value(num, 0); }
	inline plugPackageItem *getChild(const QString &id) { return m_children.value(id, 0); }
	inline int childCount() { return m_children_temp.count(); }
	void addChild(const QString &id, plugPackageItem *item);
	void removeChild(const QString &id, int number);
	inline int childNumber() { return m_child_number; }
	inline plugPackageItem *parent() { return m_parent; }
	QStringList children() { return m_children.keys(); }
private:
	int m_child_number;
	plugPackageItem *m_parent;
	packageInfo *m_package_info;
	QVector<plugPackageItem *> m_children_temp;
	QHash<QString, plugPackageItem *> m_children;
};

#endif // PLUGPACKAGEITEM_H

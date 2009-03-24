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

#include "plugpackageitem.h"

plugPackageItem::plugPackageItem() : m_child_number(0), m_parent(0)
{

}

plugPackageItem::plugPackageItem(packageInfo* item)
{
	m_package_info = item;
}

plugPackageItem::~plugPackageItem() {
	if(m_package_info)
		delete m_package_info;
	foreach(plugPackageItem *item, m_children_temp)
		delete item;
}

void plugPackageItem::addChild(const QString& id, plugPackageItem* item) {
	item->m_child_number =  m_children_temp.count();
	item->m_parent = this;
	m_children_temp.append(item);
	m_children.insert(id, item);
}

void plugPackageItem::removeChild(const QString& id, int number) {
	m_children_temp.remove(number);
	delete m_children.take(id);
	for(int i=number; i<m_children_temp.size(); i++)
		m_children_temp[i]->m_child_number--;
}

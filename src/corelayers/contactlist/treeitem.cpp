/*****************************************************************************
    Tree Item

    Copyright (c) 2008 by Nigmatullin Ruslan <euroelessar@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include "treeitem.h"
#include "src/pluginsystem.h"
#include "src/iconmanager.h"
#include <QDebug>
#include <QBitmap>
#include <QPainter>
#include <QPoint>
#include <QBrush>
#include <QRectF>
#include <QSettings>

TreeItem::TreeItem(const QVariant &display, TreeItem *parent)
{
	m_item_icons = QVector<QVariant>(13).toList();
	m_item_bottom_rows = QVector<QVariant>(3).toList();
	m_parent_item = parent;
	m_item_display = display;
	m_is_expanded=true;
	m_is_online=false;
	m_item_mass=1000;
	m_visible=0;
	m_invisible=0;
	m_is_visible=false;
	m_is_visible=true;
	m_is_always_visible=false;
	m_is_always_invisible=false;
	m_current_status_icon = IconManager::instance().getIcon("qutim");
	m_item_icons[0] = m_current_status_icon;
	m_content = 0;
	m_visibility_flags = ShowDefault;
}

TreeItem::~TreeItem()
{
	ensure_invis1();
	qDeleteAll(m_child_items);
}

TreeItem *TreeItem::child(int number)
{
	return m_child_items.value(number);
}

int TreeItem::childCount() const
{
	return m_child_items.size();//count();
}

int TreeItem::childNumber() const
{
	if (m_parent_item)
		return m_parent_item->m_child_items.indexOf(const_cast<TreeItem*>(this));

	return 0;
}

QVariant TreeItem::data(int role) const
{
	switch(role)
	{
	case Qt::DecorationRole:
                return reinterpret_cast<qptrdiff>(&m_item_icons);
	case Qt::DisplayRole:
	case Qt::EditRole:
		return m_item_display;
	case Qt::UserRole:
		return m_item_type;
	case Qt::UserRole+1:
		if(m_item_type==1)
			return m_parent_item->getChildPosition(m_structure.m_item_name);
		return m_item_mass;
	case Qt::UserRole+2:
                return reinterpret_cast<qptrdiff>(&m_item_bottom_rows);
	case Qt::UserRole+3:
		return m_item_status;
	case Qt::UserRole+4:
		return m_current_status_icon;
	case Qt::UserRole+5:
		return m_is_always_visible || m_is_visible;
	case Qt::ToolTipRole:{
		if(m_item_type!=0)
			return QVariant();
		QString tooltip =  PluginSystem::instance().getItemToolTip(m_structure);
		if(tooltip.isNull())
			return QVariant();
		return tooltip;}
	default:
		return QVariant();
	}
}

bool TreeItem::insertChildren(int position, int count)
{
	if (position < 0 || position > m_child_items.size())
		return false;

	for (int row = 0; row < count; ++row) {
		TreeItem *item = new TreeItem(QVariant(), this);
		m_child_items.insert(position, item);
	}

	return true;
}

TreeItem *TreeItem::parent()
{
	return m_parent_item;
}

bool TreeItem::removeChildren(int position, int count)
{
	if (position < 0 || position + count > m_child_items.size())
		return false;
	for (int row = 0; row < count; ++row)
	{
		delete m_child_items[position];
		m_child_items.removeAt(position);
	}

	return true;
}
bool TreeItem::removeChildren()
{
	return removeChildren(0,m_child_items.size());
}

bool TreeItem::setData(const QVariant &value, int role)
{
	switch(role)
	{
	case Qt::DecorationRole:
                m_item_icons = value.toList();
		return true;
	case Qt::UserRole:
		m_item_type = value;
		return true;
	case Qt::DisplayRole:
		m_item_display = value;
		return true;
	case Qt::UserRole+1:
		m_item_mass = value.toInt();
		return true;
	case Qt::EditRole:
		m_item_display = value;
		return true;
	case Qt::UserRole+4:
		m_current_status_icon = value;
	default:
		return false;
	}
}

void TreeItem::getData(ItemData &data)
{
	data.name = m_item_display.toString();
	data.item = m_structure;
	data.status_mass = m_item_mass;
	data.status_id = m_item_status;
	data.status = m_item_bottom_rows[0].toList().toVector();
	data.icons.clear();
	data.icons.reserve(13);
	foreach(const QVariant &icon, m_item_icons)
		data.icons.append(qvariant_cast<QIcon>(icon));
	data.attributes = m_content;
	data.visibility = m_visibility_flags;
}

void TreeItem::setStructure(const TreeModelItem &structure)
{
	m_structure = structure;
}
void TreeItem::setExpanded(bool expanded)
{
	m_is_expanded = expanded;
}
const TreeModelItem &TreeItem::getStructure()
{
	return m_structure;
}
QString TreeItem::getName()
{
	return m_structure.m_item_name;
}

int TreeItem::indexOf(TreeItem *item)
{
	return m_child_items.indexOf(item);
}

TreeItem *TreeItem::find(const QString &id)
{
	return m_item_list.value(id);
}
void TreeItem::setHash(const QString &id, TreeItem *item)
{
	m_item_list.insert(id, item);
}
void TreeItem::removeHash(const QString &id)
{
	m_item_list.remove(id);
}
void TreeItem::setImage(const QIcon &icon, int column)
{
	if(column<13&&column>-1)
	{
		if(column==1)
		{
			static int size=24;
			static QSize ava_size(size,size);
			QPixmap pixmap = icon.pixmap(icon.actualSize(QSize(65535,65535)),QIcon::Normal,QIcon::On);
			if(!pixmap.isNull())
			{
				static QPixmap alpha;
				if( alpha.isNull() )
				{
					alpha = QPixmap( ava_size );
					alpha.fill(QColor(0,0,0));
					QPainter painter(&alpha);
					QPen pen(QColor(127,127,127));
					painter.setRenderHint(QPainter::Antialiasing);
					pen.setWidth(0);
					painter.setPen(pen);
					painter.setBrush(QBrush(QColor(255,255,255)));
					painter.drawRoundedRect(QRectF(QPointF(0,0),QSize(size-1,size-1)),5,5);
					painter.end();
				}
				pixmap = pixmap.scaled(ava_size,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
				pixmap.setAlphaChannel(alpha);
				m_item_icons[column] = QIcon(pixmap);
			}
			return;
		}
		if(column==0)
			m_current_status_icon=icon;
		m_item_icons[column] = icon;
	}
}
QIcon TreeItem::getImage(int column)
{
	if(column>-1&&column<13)
	{
		return  qvariant_cast<QIcon>(m_item_icons[column]);
	}
	return QIcon();
}
void TreeItem::setRow(const QVariant &item, int row)
{
	if(row<0||row>2)
		return;
	m_item_bottom_rows[row]=item;
}
void TreeItem::setStatus(const QString &text, const QIcon &icon, int mass)
{
	m_item_icons[0]=icon;
	m_current_status_icon=icon;
	m_item_status=text;
	m_item_mass=mass;
}
bool TreeItem::isExpanded()
{
	return m_is_expanded;
}
void TreeItem::setOnline(bool show)
{
	m_is_online=show;
}
bool TreeItem::getOnline()
{
	return m_is_online;
}
int TreeItem::getChildPosition(const QString &child)
{
	if(child=="")
		return 10000;
	int position = m_child_order.indexOf(child);
	if(position<0)
	{
		position=m_child_order.size();
		m_child_order.append(child);
	}
	return position;
}
void TreeItem::moveChild(const QString &child, int position)
{
	if(child=="")
		return;
	m_child_order.removeAll(child);
	if(position<0)
		position=0;
	else if(position>m_child_order.size())
		position=m_child_order.size();
	m_child_order.insert(position, child);
}
void TreeItem::setChildOrder(const QStringList &order)
{
	m_child_order=order;
}
const QStringList &TreeItem::getChildOrder()
{
	return m_child_order;
}
void TreeItem::setAlwaysVisible(bool visible)
{
	m_is_always_visible = visible;
}
bool TreeItem::getAlwaysVisible()
{
	return m_is_always_visible;
}
void TreeItem::setAlwaysInvisible(bool invisible)
{
//	if(!m_is_always_invisible && invisible)
//		m_parent_item->m_invisible++;
//	else if(m_is_always_invisible && !invisible)
//		m_parent_item->m_invisible--;
	m_is_always_invisible = invisible;
}
bool TreeItem::getAlwaysInvisible()
{
	return m_is_always_invisible;
}

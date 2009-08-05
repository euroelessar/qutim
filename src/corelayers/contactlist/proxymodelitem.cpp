/*****************************************************************************
    Proxy Model Item

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

#include "proxymodelitem.h"
#include <QDebug>

ProxyModelItem::ProxyModelItem(const QModelIndex &source, QHash<TreeItem *, QModelIndex> *source_list, ProxyModelItem *parent)
{
	if(source.isValid())
	{
		m_item_type=0;
		m_item_mass=static_cast<TreeItem*>(m_source_index[0].internalPointer())->data(Qt::UserRole+1).toInt();
		m_item_name=static_cast<TreeItem*>(m_source_index[0].internalPointer())->data(Qt::DisplayRole).toString();
	}
	else
	{
		m_item_type=-1;
		m_item_mass=0;
		m_item_name="";
	}
	m_is_deleted=false;
	m_online_children=0;
	m_source_index.append(source);
	m_parent_item=parent;
	m_has_offline_separator=false;
	m_has_online_separator=false;
	m_mass = QPair<int, QString>(m_item_mass, m_item_name.toLower());
        m_source_list = source_list;
        m_decoration = QVector<QVariant>(13).toList();
        m_rows = QVector<QVariant>(3).toList();
        m_checksum_value = 0x12345678;
}

ProxyModelItem::~ProxyModelItem()
{
	if(m_source_list)
		foreach(QModelIndex index, m_source_index)
			if(index.isValid())
				m_source_list->remove(static_cast<TreeItem*>(index.internalPointer()));
	qDeleteAll(m_child_items);
        m_child_items.clear();
	//foreach(ProxyModelItem *item, m_child_items);
}
ProxyModelItem *ProxyModelItem::clone()
{
	ProxyModelItem *item = new ProxyModelItem(QModelIndex(),m_source_list,m_parent_item);
	for(int i=m_child_items.size()-1;i>=0;i++)
		item->insertChild(0,item);
	item->m_online_children=m_online_children;
	switch(m_item_type)
	{
	case -1:
		item->setSourceIndex(QModelIndex());
		break;
	case 0:
		item->setSourceIndex(m_source_index.at(0));
		break;
	case 1:
		for(int i=0;i<m_source_index.size();i++)
			item->appendSourceIndex(m_source_index.at(i));
		break;
	}
	item->setSeparator(0,m_has_online_separator);
	item->setSeparator(1,m_has_offline_separator);
	return item;
}
void ProxyModelItem::moveChild(int old_position, int new_position)
{
	if (old_position < 0 || old_position > m_child_items.size()-1)
		return;
	if (new_position < 0 || new_position > m_child_items.size()-1)
		return;
	ProxyModelItem *item = m_child_items.at(old_position);
	m_child_items.insert(new_position,item);
	m_child_items.removeAt(old_position);
}

ProxyModelItem *ProxyModelItem::child(int number)
{
	return m_child_items.value(number);	
}

int ProxyModelItem::childCount() const
{
	return m_child_items.count();
}

int ProxyModelItem::childNumber() const
{
        if(m_checksum_value != 0x12345678)
            return 0;
	//qWarning() << "childNumber(); m_is_deleted = " << m_is_deleted;
	if (m_parent_item)
		return m_parent_item->m_child_items.indexOf(const_cast<ProxyModelItem*>(this));
	return 0;
}

QVariant ProxyModelItem::data(int role) const
{
	switch(role)
	{
	case Qt::EditRole:
		return m_item_edit;
	case Qt::FontRole:
		return m_font;
	case Qt::UserRole+10:
		return m_color;
	}
	if(m_item_type==0||m_item_type==1)
	{
		TreeItem *item = static_cast<TreeItem*>(m_source_index[0].internalPointer());//getSourceItem();
		switch(role)
		{
		case Qt::DisplayRole:{
			int type = m_item_type==1?1:item->data(Qt::UserRole).toInt(); 
			QString ans=m_item_name;
			//qWarning() << m_item_name << " " << m_item_name.isEmpty() << m_item_type;
			if(type==1)
			{
				if(item->getName().isEmpty())// && m_item_name.isEmpty())
					ans=QObject::tr("Not in list");
				int online=m_online_children;//childCount();
				/*if(m_has_offline_separator)
					online--;
				if(m_has_online_separator)
					online--;*/
				int num=0;
				foreach(QModelIndex index, m_source_index)
					num+=static_cast<TreeItem*>(index.internalPointer())->childCount()-static_cast<TreeItem*>(index.internalPointer())->m_invisible;
				ans+=" ("+QString().setNum(online)+"/"+QString().setNum(num)+")";
			}
			return ans;}
		case Qt::UserRole+1:
			return m_item_mass;
		default:
			return item->data(role);
		}
	}
	else
		switch(role)
		{
		case Qt::DisplayRole:
			return m_item_name;
		case Qt::UserRole:
			return m_item_type;
		case Qt::UserRole+1:
			return m_item_mass;
		case Qt::DecorationRole:
                        return reinterpret_cast<qptrdiff>(&m_decoration);
		case Qt::UserRole+2:
                        return reinterpret_cast<qptrdiff>(&m_rows);
		case Qt::UserRole+4:
			return QVariant();
		default:
			return QVariant();
		}
}
bool ProxyModelItem::setData(const QVariant &value, int role)
{
	switch(role)
	{
	case Qt::EditRole:
		m_item_edit = value.toString();
		break;
	case Qt::FontRole:
		m_font = value;
		break;
	case Qt::UserRole+10:
		m_color = value;
		break;
	default:
		return false;
	}
	return true;
}

bool ProxyModelItem::insertChildren(int position, int count)
{
	if (position < 0 || position > m_child_items.size())
		return false;

	for (int row = 0; row < count; ++row) {
		ProxyModelItem *item = new ProxyModelItem(QModelIndex(), m_source_list, this);
		m_child_items.insert(position, item);
	}

	return true;
}

ProxyModelItem *ProxyModelItem::parent()
{
	return m_parent_item;
}

bool ProxyModelItem::removeChildren(int position, int count)
{
	if (position < 0 || position + count > m_child_items.size())
		return false;
	for (int row = 0; row < count; ++row)
	{
		m_child_items[position]->removeChildren();
		delete m_child_items[position];
		//m_child_items[position]->deleteItem();
		m_child_items.removeAt(position);
	}

	return true;
}
bool ProxyModelItem::removeChildren()
{
	if(m_child_items.size()<1)
		return true;
	return removeChildren(0,m_child_items.size());
}
bool ProxyModelItem::insertChild(int position, ProxyModelItem *item)
{
	if (position < 0 || position > m_child_items.size())
		return false;
	m_child_items.insert(position, item);
	return true;
}
QModelIndex ProxyModelItem::getSourceIndex()
{
	if(m_source_index.size()>0)
		return m_source_index.at(0);
	else
		return QModelIndex();
}
/*TreeItem *ProxyModelItem::getSourceItem()
{
	return static_cast<TreeItem*>(m_source_index.internalPointer());
}*/
QList<QModelIndex> ProxyModelItem::getSourceIndexes()
{
	return m_source_index;
}
int ProxyModelItem::getType()
{
	return m_item_type;
}
void ProxyModelItem::setSourceIndex(const QModelIndex &source)
{
	m_source_index.clear();
	if(source.isValid())
		m_item_type=0;
	else
		m_item_type=-1;
	m_source_index.append(source);	
}
void ProxyModelItem::appendSourceIndex(const QModelIndex &source)
{
	m_item_type=1;
	if(!m_source_index[0].isValid())
	{
		m_source_index[0]=source;
		return;
	}
	if(m_source_index.indexOf(source)<0)
		m_source_index.append(source);
}
void ProxyModelItem::setName(const QString &name)
{
	m_item_name = name;
	m_item_edit = name;
	m_mass.second = name.toLower();
}
bool ProxyModelItem::operator >(ProxyModelItem *arg)
{
	int dmass = m_item_mass - arg->getMass(); 
	if(dmass>0)
		return true;
	else if(dmass==0)
		return m_item_name.compare(arg->getName());
	return false;
}
int ProxyModelItem::getMass()
{
	return m_item_mass;
}
void ProxyModelItem::setMass(int mass)
{
	m_item_mass=mass;
	m_mass.first = mass;
}
QString ProxyModelItem::getName()
{
	return m_item_name;
}
bool ProxyModelItem::getSeparator(int type)
{
	switch(type)
	{
	case 0:
		return m_has_online_separator;
	case 1:
		return m_has_offline_separator;
	case 2:
		return m_has_nil_separator;
	default:
		return false;
	}
}
void ProxyModelItem::setSeparator(int type, bool value)
{
	switch(type)
	{
	case 0:
		m_has_online_separator=value;
		break;
	case 1:
		m_has_offline_separator=value;
		break;
	case 2:
		m_has_nil_separator=value;
	default:
		break;
	}
}
void ProxyModelItem::deleteItem()
{
	foreach(ProxyModelItem *item, m_child_items)
		item->deleteItem();
	m_is_deleted=true;
}

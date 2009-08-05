/*
    Conference Item Model

    Copyright (c) 2008 by Nigmatullin Ruslan <euroelessar@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#include "conferenceitemmodel.h"

ConferenceItemModel::ConferenceItemModel(ConfContactList *contact_list, QObject *parent)
	: QAbstractItemModel(parent)
{
	m_contact_list = contact_list;
}

ConferenceItemModel::~ConferenceItemModel()
{
}

QVariant ConferenceItemModel::data(const QModelIndex &index, int role) const
{
	if(index.column()!=0)
		return QVariant();
	if (!index.isValid())
		return QVariant();

	ConferenceItem *item = getItem(index);

	return item->data(role);
}

QVariant ConferenceItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(section!=0)
		return QVariant();
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole && section==0)
		return QString("test");

	return QVariant();
}

QModelIndex ConferenceItemModel::index(int row, int column, const QModelIndex &parent) const
{
	if(parent.isValid() || m_item_list.size()<=row || row<0)
		return QModelIndex();
	else
		return createIndex(row, column, m_item_list[row]);
}

QModelIndex ConferenceItemModel::parent(const QModelIndex &index) const
{
	return QModelIndex();
}

int ConferenceItemModel::rowCount(const QModelIndex &parent) const
{
	return m_item_list.size();
}

int ConferenceItemModel::columnCount(const QModelIndex &parent) const
{
	return 1;
}

Qt::ItemFlags ConferenceItemModel::flags(const QModelIndex &index) const
{
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool ConferenceItemModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
	return false;
}

bool ConferenceItemModel::insertColumns(int position, int columns, const QModelIndex &parent)
{
	return false;
}

bool ConferenceItemModel::removeColumns(int position, int columns, const QModelIndex &parent)
{
	return false;
}

bool ConferenceItemModel::insertRows(int position, int rows, const QModelIndex &parent)
{
	if(position>m_item_list.size()+1)
		return false;
	beginInsertRows(parent, position, position + rows - 1);
	for(int i=position;i<position+rows;i++)
		m_item_list.insert(i,new ConferenceItem("",m_contact_list));
	endInsertRows();
	return true;
}

bool ConferenceItemModel::removeRows(int position, int rows, const QModelIndex &parent)
{
	if(position<0 || position+rows>m_item_list.size())
		return false;
	
	beginRemoveRows(parent, position, position + rows - 1);
	for(int i=0;i<rows;i++)
	{
		m_item_hash.remove(m_item_list[position]->data(Qt::DisplayRole).toString());
		delete m_item_list[position];
		m_item_list.removeAt(position);
	}
	endRemoveRows();
	return true;
}
	
bool ConferenceItemModel::addBuddy(const QString & name)
{
	//if(!insertRows(0,1,QModelIndex()))
	//	return false;
	if(m_item_hash.contains(name))
		return false;
	ConferenceItem *item = new ConferenceItem(name, m_contact_list);
	m_item_hash[name] = item;
	int pos = position( item );
	beginInsertRows( QModelIndex(), pos, pos );
	m_item_list.insert( pos, item );
	endInsertRows();
	//position(item);
	return true;
}

bool ConferenceItemModel::removeBuddy(const QString & name)
{
	ConferenceItem *item = m_item_hash.value(name,0);
	if(!item)
		return false;
	return removeRows(m_item_list.indexOf(item),1,QModelIndex());
}
	
bool ConferenceItemModel::renameBuddy(const QString & name, const QString & new_name)
{
	ConferenceItem *item = m_item_hash.value(name,0);
	if(!item)
		return false;
	m_item_hash.remove(name);
	item->setData(new_name,Qt::DisplayRole);
	m_item_hash.insert(new_name,item);
	position(item);
	return true;
}

bool ConferenceItemModel::setItemIcon(const QString & name, const QIcon & icon, int icon_position)
{
	ConferenceItem *item = m_item_hash.value(name,0);
	if(!item)
		return false;
	item->setImage(icon, icon_position);
	if(icon_position == 1)
	{
		 int pos = m_item_list.indexOf(item);
		 beginRemoveRows( QModelIndex(), pos, pos );
		 m_item_list.removeAt( pos );
		 endRemoveRows();
		 beginInsertRows( QModelIndex(), pos, pos );
		 m_item_list.insert( pos, item );
		 endInsertRows();
	}
	else
	{
		QModelIndex index = createIndex(m_item_list.indexOf(item),0,item);
		emit dataChanged(index,index);
	}
	return true;
}

bool ConferenceItemModel::setItemRow(const QString & name, const QList<QVariant> & var, int row)
{
	ConferenceItem *item = m_item_hash.value(name,0);
	if(!item)
		return false;
	item->setRow(QVariant(var), row);
	QModelIndex index = createIndex(m_item_list.indexOf(item),0,item);
	emit dataChanged(index,index);
	changePersistentIndex( index, index );
	return true;
}

bool ConferenceItemModel::setItemStatus(const QString & name, const QIcon & icon, const QString & status, int mass)
{
	ConferenceItem *item = m_item_hash.value(name,0);
	if(!item)
		return false;
	item->setStatus(status, icon, mass);
	int old_pos = m_item_list.indexOf( item );
	int pos = position( item );
	QModelIndex old_index = createIndex( old_pos, 0, item );
	if( old_pos == pos )
	{
		emit dataChanged( old_index, old_index );
		return true;
	}
	QModelIndex new_index = createIndex( pos, 0, item );
	m_item_list.move( old_pos, pos );
	changePersistentIndex( old_index, new_index );
	return true;
}

bool ConferenceItemModel::setItemRole(const QString & name, const QIcon & icon, const QString & role, int mass)
{
	ConferenceItem *item = m_item_hash.value(name,0);
	if(!item)
		return false;
	item->setRole(role, icon, mass);
	int old_pos = m_item_list.indexOf( item );
	int pos = position( item );
	QModelIndex old_index = createIndex( old_pos, 0, item );
	if( old_pos == pos )
	{
		emit dataChanged( old_index, old_index );
		return true;
	}
	QModelIndex new_index = createIndex( pos, 0, item );
	m_item_list.move( old_pos, pos );
	changePersistentIndex( old_index, new_index );
	return true;
}

bool ConferenceItemModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if(index.column()!=0)
		return false;

	ConferenceItem *item = getItem(index);

	bool result = item->setData( value, role );
	 if( result )
		 emit dataChanged( index, index );
	 return result;
}

ConferenceItem *ConferenceItemModel::getItem(const QModelIndex &index) const
{
	if (index.isValid()){
		ConferenceItem *item = static_cast<ConferenceItem*>(index.internalPointer());
		if (item) 
			return item;
	}
	return 0;
}

int ConferenceItemModel::position(ConferenceItem *item)
{
	int mass = item->getMass();
	int k = 0;
	QString string = item->data(Qt::DisplayRole).toString();
	for(int i=0;i<m_item_list.size();i++)
	{
	    int item_mass=m_item_list[i]->getMass();
	    if( m_item_list[i] == item )
		k = -1;
	     else if( (item_mass>mass) || (item_mass==mass && m_item_list[i]->data
					   (Qt::DisplayRole).toString().compare(string,Qt::CaseInsensitive)>0) )
	    {
		 return i + k;
	     }
	 }
	return m_item_list.size() + k;
}

QStringList ConferenceItemModel::getUsers()
{
	return m_item_hash.keys();
}

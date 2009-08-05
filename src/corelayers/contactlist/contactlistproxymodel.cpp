/*****************************************************************************
    Contact List Proxy Model

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

#include "contactlistproxymodel.h"
#include "src/pluginsystem.h"
#include <QtGui>

bool compareItem(ProxyModelItem *item1, ProxyModelItem *item2)
{
	return item1->m_mass < item2->m_mass;
}

ContactListProxyModel::ContactListProxyModel(QObject *parent) : QAbstractProxyModel(parent)
{
	m_root_item = new ProxyModelItem(QModelIndex(), &m_source_list);
	//qWarning() << "Root item:" << m_root_item;
	m_model_type = 0;
	m_show_offline=true;
	m_show_empty_group=true;
	m_sort_status=false;
	m_show_separator=true;//false;
	m_position_buddy = new ProxyModelItem(QModelIndex(), 0);
	m_empty_model = new QStandardItemModel();
	m_append_to_expand_list = false;
}
void ContactListProxyModel::setSettings(int type, bool show_offline, bool show_empty, bool sort_status, bool show_separator, const QVariant & account_font, const QVariant & group_font, const QVariant & online_font, const QVariant & offline_font, const QVariant & separator_font, const QVariant & account_color, const QVariant & group_color, const QVariant & online_color, const QVariant & offline_color, const QVariant & separator_color)
{
	m_model_type = type%4;
	m_show_offline = show_offline;
	m_show_empty_group = show_empty;
	m_sort_status = sort_status;
	m_show_separator = show_separator;	
	m_account_font = account_font;
	m_group_font = group_font;
	m_online_font = online_font;
	m_offline_font = offline_font;
	m_separator_font = separator_font;
	m_account_color = account_color;
	m_group_color = group_color;
	m_online_color = online_color;
	m_offline_color = offline_color;
	m_separator_color = separator_color;
        removeAllItems();
}
void ContactListProxyModel::loadProfile(const QString & profile_name)
{
	m_profile_name=profile_name;
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
	settings.beginGroup("contactlist");
	setChildOrder(settings.value("grouporder",QStringList()).toStringList());
	m_is_expanded = settings.value("expandedgroups",QStringList()).toStringList();
	settings.endGroup();
}
void ContactListProxyModel::saveSettings()
{
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
	settings.beginGroup("contactlist");
	settings.setValue("grouporder",getChildOrder());
	settings.setValue("expandedgroups",m_is_expanded);
	settings.endGroup();	
}

ContactListProxyModel::~ContactListProxyModel()
{
	delete m_root_item;
	delete m_position_buddy;
	delete m_empty_model;
}
QModelIndex ContactListProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
	ProxyModelItem *item = getItem(proxyIndex);
	return item->getSourceIndex();
}
QModelIndex ContactListProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
	QModelIndex proxy = m_source_list.value(reinterpret_cast<TreeItem*>(sourceIndex.internalPointer()),QModelIndex());
	if(!proxy.isValid())
		return QModelIndex();
	ProxyModelItem *item = getItem(proxy);
	if(!item)
		return QModelIndex();
	proxy=proxy.sibling(item->childNumber(),0);
	return proxy;
}
int ContactListProxyModel::columnCount(const QModelIndex &parent) const
{
	return 1;
}
QVariant ContactListProxyModel::data(const QModelIndex &index, int role) const
{
	if(index.column()!=0)
		return QVariant();
	if (!index.isValid())
		return QVariant();

	ProxyModelItem *item = getItem(index);

	return item->data(role);
}
bool ContactListProxyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if(role!=Qt::EditRole)
		return false;
	getItem(index)->setData(value,role);
	return true;
}

Qt::ItemFlags ContactListProxyModel::flags(const QModelIndex &index) const
{
	Qt::ItemFlags default_flags = Qt::ItemIsEnabled;
	int type = index.isValid()?index.data(Qt::UserRole).toInt():-1000; 
	switch(m_model_type)
	{
	case 0:
		if (!index.isValid())
			return Qt::ItemIsEnabled;
		//default_flags |= Qt::ItemIsDragEnabled;
		if(type<2)
			default_flags |= Qt::ItemIsDragEnabled;
		if(type>0)
			default_flags |= Qt::ItemIsDropEnabled;
		if(type>-1)
			default_flags |= Qt::ItemIsSelectable;
//		if(type==0 || type==1)
//			default_flags |= Qt::ItemIsEditable;
		break;
	case 1:
		if (!index.isValid())
			return Qt::ItemIsEnabled | Qt::ItemIsDropEnabled;
		if(type>-1)
			default_flags |= Qt::ItemIsSelectable;
		if(type==1)
			default_flags |= Qt::ItemIsDropEnabled;
		if(type>-1)
			default_flags |= Qt::ItemIsDragEnabled;
//		if(type==0 || type==1)
//			default_flags |= Qt::ItemIsEditable;
		break;
	case 2:
	case 3:
		if (!index.isValid())
			return Qt::ItemIsEnabled;
		if(type==0)
			default_flags |= Qt::ItemIsDragEnabled;
		if(type==1)
			default_flags |= Qt::ItemIsDropEnabled;
//		if( type < 2 && type > -1 )
//			default_flags |= Qt::ItemIsEditable;
		if(type>-1)
			default_flags |= Qt::ItemIsSelectable;
		break;
	default:
		break;
	}
	return default_flags;
}
Qt::DropActions ContactListProxyModel::supportedDropActions() const
{
	return Qt::CopyAction;
}
bool ContactListProxyModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
	if (action == Qt::IgnoreAction)
		 return true;
	if (!data->hasFormat("application/qutim.item"))
		return false;
	if (column > 0)
		return false;
	QByteArray encoded_data = data->data("application/qutim.item");
	QDataStream stream(&encoded_data, QIODevice::ReadOnly);
	quint64 item_row,item_column;
	qint64 item_id;
	stream >> item_row >> item_column >> item_id;
	QModelIndex index = createIndex((int)item_row,(int)item_column,(quint32)item_id);
	if(!index.isValid())
		return false;
	int type = index.data(Qt::UserRole).toInt();
	int parent_type = parent.data(Qt::UserRole).toInt();
	switch(m_model_type)
	{
	case 0:
		switch(type)
		{
		case 0: if(parent_type==1){
			TreeModelItem old_item = static_cast<TreeItem*>(getItem(index)->getSourceIndex().internalPointer())->getStructure();
			TreeModelItem new_item = old_item;
			new_item.m_account_name = static_cast<TreeItem*>(getItem(parent)->getSourceIndex().internalPointer())->getStructure().m_account_name;
			if(new_item.m_account_name!=old_item.m_account_name)
				return false;
			new_item.m_parent_name = static_cast<TreeItem*>(getItem(parent)->getSourceIndex().internalPointer())->getStructure().m_item_name;
			if(new_item.m_parent_name==old_item.m_parent_name)
				return false;
			PluginSystem::instance().moveItemSignalFromCL(old_item, new_item);
			return false;}
		case 1:
			if(parent_type==2){
				if(parent!=index.parent())
					return false;
				TreeItem *group = static_cast<TreeItem*>(getItem(index)->getSourceIndex().internalPointer());
				group->parent()->moveChild(group->getStructure().m_item_name,row);
				m_tree_view->setUpdatesEnabled(false);
				removeAllItems();
				m_tree_view->setUpdatesEnabled(true);
				return false;
			}
			return false;
		default:
			return false;
		}
	case 1:
		switch(type)
		{
		case 0:
			return false;
		case 1:
			if(parent==index.parent())
			{
				moveChild(getItem(index)->getName(),row);
				m_tree_view->setUpdatesEnabled(false);
                                removeAllItems();
				m_tree_view->setUpdatesEnabled(true);				
			}
			return false;
		}
		return false;
	default:
		return false;
	}
}
QStringList ContactListProxyModel::mimeTypes() const
{
	QStringList types;
	types << "application/qutim.item";
	return types;
}
QMimeData *ContactListProxyModel::mimeData(const QModelIndexList &indexes) const
{
	QMimeData *mime_data = new QMimeData();
	if(indexes.size()==0)
		return mime_data;
	QModelIndex index = indexes.at(0);
	if(!index.isValid())
		return mime_data;
	int type = index.data(Qt::UserRole).toInt();
	if(type<0 || type>2)
		return mime_data;
	if(type==0)
		mime_data->setText(static_cast<TreeItem*>(getItem(index)->getSourceIndex().internalPointer())->getStructure().m_item_name);
	QByteArray encoded_data;	
	QDataStream stream(&encoded_data, QIODevice::WriteOnly);
	stream << (quint64)index.row() << (quint64)index.column() << (qint64)index.internalId();
	mime_data->setData("application/qutim.item",encoded_data);
	return mime_data;
}
ProxyModelItem *ContactListProxyModel::getItem(const QModelIndex &index) const
{
	if (index.isValid()){
                ProxyModelItem *item = reinterpret_cast<ProxyModelItem*>(index.internalPointer());
		if (item) 
			return item;
                return 0;
	}
	return m_root_item;
}
QVariant ContactListProxyModel::headerData(int section, Qt::Orientation orientation,
								int role) const
{
	if(section!=0)
		return QVariant();
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole && section==0)
		return m_root_item->data(role);

	return QVariant();
}

QModelIndex ContactListProxyModel::index(int row, int column, const QModelIndex &parent) const
{
	if (parent.isValid() && parent.column() != 0)
		return QModelIndex();

	ProxyModelItem *parentItem = getItem(parent);

	ProxyModelItem *childItem = parentItem->child(row);
	if (childItem)
		return createIndex(row, column, childItem);
	else
		return QModelIndex();
}

bool ContactListProxyModel::insertColumns(int position, int columns, const QModelIndex &parent)
{
	return false;
}

bool ContactListProxyModel::insertRows(int position, int rows, const QModelIndex &parent)
{
	ProxyModelItem *parentItem = getItem(parent);
	bool success;
	beginInsertRows(parent, position, position + rows - 1);
	success = parentItem->insertChildren(position, rows);
	return success;
}

QModelIndex ContactListProxyModel::parent(const QModelIndex &index) const
{
	if (!index.isValid())
		return QModelIndex();

	ProxyModelItem *childItem = getItem(index);
        if(childItem->getChecksum() != 0x12345678)
            return QModelIndex();
	if(!childItem)
		return QModelIndex();
		
	ProxyModelItem *parentItem = childItem->parent();
        if(!parentItem || parentItem->getChecksum() != 0x12345678)
		return QModelIndex();
	
	if (parentItem == m_root_item)
		return QModelIndex();
	return createIndex(parentItem->childNumber(), 0, parentItem);
}

bool ContactListProxyModel::removeColumns(int position, int columns, const QModelIndex &parent)
{
	return false;
}

bool ContactListProxyModel::removeRows(int position, int rows, const QModelIndex &parent)
{
	ProxyModelItem *parent_item = getItem(parent);
	if(!parent_item)
		return false;
	if(position<0 || position+rows>parent_item->childCount())
		return false;
	if(rows<1)
		return false;
	bool success = true;
	/*for(int i=position;i<position+rows;i++)
	{
		ProxyModelItem *item = parent_item->child(i);
		QModelIndexList index_list = item->getSourceIndexes();
		foreach(QModelIndex index, index_list)
		{
			m_source_list.remove(static_cast<TreeItem*>(index.internalPointer()));
		}
	}*/
	beginRemoveRows(parent, position, position + rows - 1);
	success = parent_item->removeChildren(position, rows);
	endRemoveRows();
	
	return success;
}

int ContactListProxyModel::rowCount(const QModelIndex &parent) const
{
	ProxyModelItem *parentItem = getItem(parent);

	return parentItem->childCount();
}
void ContactListProxyModel::setModel(TreeContactListModel *model)
{
	m_tree_model = model;
}
void ContactListProxyModel::insertItem(const QModelIndex &source)
{
	if( !source.isValid() )
		return;
	TreeItem *source_item = static_cast<TreeItem*>(source.internalPointer());
	QString name = source_item->data(Qt::DisplayRole).toString();
	int mass = source_item->data(Qt::UserRole+1).toInt();
	int visibility = source_item->getItemVisibility();
	int attributes = source_item->getAttributes();
	const TreeModelItem &structure = source_item->getStructure();
	int source_type = structure.m_item_type;
	bool show = false;
	if( source_type == 0 )
	{
		show |= ( mass == 1000 ) && ( visibility & ShowOffline || m_show_offline );
		show |= ( mass < 1000 )  && ( visibility & ShowOnline );
		show |= ( attributes & ItemHasUnreaded )      && ( visibility & ShowMessage );
		show |= ( attributes & ItemHasChangedStatus ) && ( visibility & ShowStatus );
		show |= ( attributes & ItemIsTyping )         && ( visibility & ShowTyping );
	}
	else
		show = true;
	if( !show )
		return;
	QModelIndex proxy_index = mapFromSource(source.parent());
	int position=getItem(proxy_index)->childCount();
	QVariant font;
	QVariant color;
	if(source_type==0)
	{
		font=mass==1000?m_offline_font:m_online_font;
		color=mass==1000?m_offline_color:m_online_color;
	}
	else if(source_type==1)
	{
		font=m_group_font;
		color=m_group_color;
	}
	else if(source_type==2)
	{
		font=m_account_font;
		color=m_account_color;
	}
	if(source_type==0)
	{
		if(!m_sort_status && mass<1000)
			mass=0;
		if(m_model_type==2 || m_model_type==3)
			if(source_item->parent()->data(Qt::DisplayRole).toString().isEmpty())
				mass+=2000;
		position = findPosition(proxy_index,name,mass);
	}
	else if(source_type==1)
		position = findPosition(proxy_index,name,mass);		
	else if(source_type==2)
		mass=-100;
	switch(m_model_type)
	{
	case 2: // With accounts, without groups
	case 0:{ // Standart view
		if(source_type==0 && m_model_type==2)
		{
			proxy_index = mapFromSource(source.parent().parent());
			position = findPosition(proxy_index,name,mass);
		}
		if(!proxy_index.isValid() && source_type!=2)
		{
			insertItem(source.parent());
			proxy_index = mapFromSource(source.parent());
			position = findPosition(proxy_index,name,mass);
		}
		switch(source_type)
		{
		case 1:
			if(m_model_type==2)
				return;
			if(!m_show_empty_group && ((source_item->m_visible+m_show_offline?source_item->childCount():0)==0))
				return;
		case 0:
		case 2:{
//			emit layoutAboutToBeChanged();
			m_tree_view->setUpdatesEnabled(false);
			if(position>getItem(proxy_index)->childCount())
				position=getItem(proxy_index)->childCount();
			else if(position<0)
				position=0;
			bool expanded = false;
			if( source_type > 0 )
				expanded = source_item->isExpanded();
			insertRows(position,1,proxy_index);
			ProxyModelItem *item = getItem(proxy_index)->child(position);
			item->setSourceIndex(source);
			item->setMass(mass);
			item->setName(name);
			item->setData(font,Qt::FontRole);
			item->setData(color,Qt::UserRole+10);
			endInsertRows();
			if(mass!=1000 && source_type==0)
				item->parent()->m_online_children++;
			m_source_list.insert(source_item,createIndex(0,0,item));
			if( source_type > 0 && expanded )
			{
				if( m_append_to_expand_list )
					m_list_for_expand.append(item);
				else
					m_tree_view->setExpanded( createIndex_(item), true );
			}
			m_tree_view->setUpdatesEnabled(true);
//			emit layoutChanged();
			break;
		}
		}
		break;
	}
	case 3: // without accounts, wihtout groups
		if(source_type==1 || source_type==2)
			return;			
	case 1:{ // View without separate to accounts
		TreeItem *source_item = static_cast<TreeItem*>(source.internalPointer());
		int source_type = source_item->data(Qt::UserRole).toInt();
		if(source_type==2)
			return;
		/*if(source_type==1)
		{
			if(!m_show_empty_group && source_item->childCount()==0)
				return;
		}*/
		// Try to find exists group with needed name
		QString group_name;
		if(source_type==1)
			group_name = source_item->data(Qt::DisplayRole).toString();
		else
			group_name = source_item->parent()->data(Qt::DisplayRole).toString();
		//if(m_model_type==3 && !group_name.isEmpty())
		//	group_name=tr("In list");
		ProxyModelItem *parent_item;
		QModelIndex group_index;
		if(m_model_type==1)
		{
			bool found=false;
			for(int i=0;i<m_root_item->childCount();i++)
			{
				ProxyModelItem *group = m_root_item->child(i);
				QString name = group->getName();
				//if(m_model_type==3 && !name.isEmpty())
				//	name=tr("In list");
				if(name==group_name) // if group exists, break
				{
					parent_item=group;
					found=true;
					break;
				}
			}
			if(source_type==1)			
				group_index = createIndex(0,0,static_cast<TreeItem*>(source.internalPointer()));
			else
				group_index = createIndex(0,0,static_cast<TreeItem*>(source.parent().internalPointer()));
			if(!m_show_empty_group && m_model_type==1 && source_type==1 && static_cast<TreeItem*>(group_index.internalPointer())->m_visible==0)
				return;
			if(!found)// So create new group
			{
//				emit layoutAboutToBeChanged();
				bool expanded=isExpanded(group_name);
				int group_mass = getChildPosition(group_name);
				position = findPosition(QModelIndex(), group_name, group_mass);
				if(position>m_root_item->childCount())
					position=m_root_item->childCount();
				else if(position<0)
					position=0;
				insertRows(position,1,QModelIndex());
				parent_item = m_root_item->child(position);
				if(m_model_type==1 || group_name=="")
				{
					parent_item->setSourceIndex(group_index);
					m_source_list.insert(static_cast<TreeItem*>(group_index.internalPointer()),createIndex(0,0,m_root_item->child(position)));
				}
				parent_item->setMass(group_mass);
				parent_item->setName(group_name);
				parent_item->setData(font,Qt::FontRole);
				parent_item->setData(color,Qt::UserRole+10);
				endInsertRows();
				if(expanded)
				{
					if( m_append_to_expand_list )
						m_list_for_expand.append( parent_item );
					else
						m_tree_view->setExpanded( createIndex(position,0,parent_item), true );
				}
//				emit layoutChanged();
			}
			if(!m_source_list.contains(static_cast<TreeItem*>(group_index.internalPointer())))
			{
				parent_item->appendSourceIndex(group_index);
				m_source_list.insert(static_cast<TreeItem*>(group_index.internalPointer()),createIndex_(parent_item));
			}
			if(source_type==1) // This is all for creating group
				return;
		}
		else
		{
			parent_item = m_root_item;
		}
//		emit layoutAboutToBeChanged();
		//QModelIndex proxy_index = mapFromSource(source.parent());//mapFromSource(createIndex(0,0,source_item->parent()));
		int position = findPosition(createIndex_(parent_item),name,mass);
		insertRows(position,1,createIndex_(parent_item));
		ProxyModelItem *item = parent_item->child(position);
		item->setSourceIndex(source);
		item->setName(name);
		item->setMass(mass);
		item->setData(font,Qt::FontRole);
		item->setData(color,Qt::UserRole+10);
		endInsertRows();
		if(mass!=1000)
			item->parent()->m_online_children++;
		m_source_list.insert(source_item,createIndex(0,0,item));
		proxy_index = createIndex_(parent_item);
//		emit layoutChanged();
		break;
	}
	}	
	position=getItem(proxy_index)->childCount();
	//return;
	if(source_type==0)
	{
		if(!m_sort_status && mass<1000)
			mass=0;
		// Add separator if need
		if(m_show_separator)
			if((mass==1000 && !getItem(proxy_index)->getSeparator(1)) || (mass>-1 && mass<999 && !getItem(proxy_index)->getSeparator(0)) || (mass>1999 && !getItem(proxy_index)->getSeparator(2) ))
			{
//				emit layoutAboutToBeChanged();
				int separator_mass;
				//int separator_position;
				QString separator_name;
				if(mass<1000)
				{
					getItem(proxy_index)->setSeparator(0,true);
					separator_mass=-1;
					separator_name=tr("Online");
				}
				else if(mass==1000)
				{
					getItem(proxy_index)->setSeparator(1,true);
					separator_mass=999;		
					separator_name=tr("Offline");			
				}
				else
				{
					getItem(proxy_index)->setSeparator(2,true);
					separator_mass=1500;
					separator_name=tr("Not in list");					
				}
				//getItem(proxy_index)->setSeparator(mass==1000?1:0,true);
				// Mass of offline separator is 999, of online -1
				//int separator_mass= (mass==1000)?999:-1;
				int separator_position = findPosition(proxy_index,"",separator_mass);
				insertRows(separator_position,1,proxy_index);
				ProxyModelItem *item = getItem(proxy_index)->child(separator_position);
				item->setMass(separator_mass);
				//item->setName(mass==1000?tr("Offline"):tr("Online"));
				item->setName(separator_name);
				item->setData(m_separator_font,Qt::FontRole);
				item->setData(m_separator_color,Qt::UserRole+10);
				endInsertRows();
//				emit layoutChanged();				
			}
	}
}
void ContactListProxyModel::removeItem(const QModelIndex &source)
{
	QModelIndex proxy = mapFromSource(source);
	if(!proxy.isValid())
		return;
//	emit layoutAboutToBeChanged();
	TreeItem *source_item = static_cast<TreeItem*>(source.internalPointer());
	TreeItem *source_parent_item = source_item->parent();
	ProxyModelItem *item = getItem(proxy);
	ProxyModelItem *parent_item = item->parent();
	int source_type = source_item->data(Qt::UserRole).toInt();
	if(source_type==0)
	{
		if(item->getMass()!=1000)
			item->parent()->m_online_children--;
		int position = item->childNumber();
		// Remove separator lines if it is last online/offline contact
		if(m_show_separator)
		{
			int mass = item->getMass();
			if(mass<1000 && parent_item->getSeparator(0))
			{				
				if(position==1)
				{
					if(position==(parent_item->childCount()-1) || parent_item->child(position+1)->getMass()==999 || parent_item->child(position+1)->getMass()==1500)
					{
						removeRows(position-1,1,createIndex_(parent_item));
						parent_item->setSeparator(0,false);						
					}
				}				
			}
			else if(mass==1000 && parent_item->getSeparator(1))
			{
				if(parent_item->child(position-1)->getMass()==999)
				{
					if(position==(parent_item->childCount()-1) || parent_item->child(position+1)->getMass()==1500)
					{
						removeRows(position-1,1,createIndex_(parent_item));
						parent_item->setSeparator(1,false);
					}					
				}			
			}
			else if(parent_item->getSeparator(2))
			{
				if(parent_item->child(position-1)->getMass()==1500)
				{
					if(position==(parent_item->childCount()-1))
					{
						removeRows(position-1,1,createIndex_(parent_item));
						parent_item->setSeparator(2,false);						
					}					
				}				
			}
			/*if(item->getMass()==1000 && position==parent_item->childCount()-1)
			{
				if(position==1 || position>0 && parent_item->child(position-1)->getMass()==999)
				{
					removeRows(position-1,1,createIndex(0,0,item->parent()));
					item->parent()->setSeparator(1,false);
				}
			}
			else if(position==1)
				if(parent_item->childCount()==2 || parent_item->childCount()>2 && parent_item->child(2)->getMass()==999)
				{
					removeRows(0,1,createIndex(0,0,item->parent()));
					item->parent()->setSeparator(0,false);
				}*/
		}
	}
	switch(m_model_type)
	{
	case 0:
		m_source_list.remove(source_item);
		removeRows(item->childNumber(),1,createIndex_(item->parent()));
		if(source_type==0)
		{
			if(!m_show_empty_group && parent_item->childCount()==0)
			{
				m_source_list.remove(source_parent_item);
				removeRows(parent_item->childNumber(),1,createIndex_(parent_item->parent()));				
			}
		}
		break;
	case 2:
		m_source_list.remove(source_item);
		removeRows(item->childNumber(),1,createIndex_(item->parent()));		
		break;
	case 1:
	case 3:{
		if(source_type==2)
			break;
		ProxyModelItem *group = item;
		TreeItem *source_group;
		if(source_type==1)
		{
			group = item;
			source_group = source_item;
		}
		else
		{
			group = item->parent();
			source_group = source_item->parent();
		}		
		if(source_type==0)
			removeRows(item->childNumber(),1,createIndex_(item->parent()));
		if(m_model_type==3)
			break;
		if(source_type==1 || (!m_show_empty_group && group->childCount()==0)) //(!m_show_offline && source_group->m_visible==0 && group->m_online_children==0 && group->childCount()==0 || m_show_offline && source_group->childCount()==0)))
		{
			int num = group->childCount();
			for(int i=0;i<num;i++)
			{
				QModelIndex child_index = group->getSourceIndex();
				if(child_index.isValid())
					m_source_list.remove(static_cast<TreeItem*>(child_index.internalPointer()));				
				//removeRows(0,1,createIndex(0,0,group));
			}
			m_source_list.remove(source_group);
			removeRows(group->childNumber(),1,createIndex_(group->parent()));
		}
		break;}
	}
	/*if(!m_show_empty_group && source_type==0 && parent_item->childCount()==0)
	{
		QList<QModelIndex> list = parent_item->getSourceIndexes();
		foreach(QModelIndex index, list)
			m_source_list.remove(static_cast<TreeItem*>(index.internalPointer()));		
		removeRows(parent_item->childNumber(),1,createIndex(0,0,parent_item->parent()));		
	}*/
	m_source_list.remove(static_cast<TreeItem*>(source.internalPointer()));
	//qWarning() << "after remove" << m_source_list.keys();
//	emit layoutChanged();
}
void ContactListProxyModel::setName(const QModelIndex &source, const QString &value)
{
	if(!source.isValid())
		return;	
	TreeItem *source_item = static_cast<TreeItem*>(source.internalPointer());
	//qWarning() << source_item;
	int source_type = source_item->data(Qt::UserRole).toInt();
	if(source_type==0)
	{
		removeItem(source);
		insertItem(source);
	}
	else
	{
		QModelIndex proxy_index = mapFromSource(source);
		if(!proxy_index.isValid())
			return;
		ProxyModelItem *item = getItem(proxy_index);
		item->setName(value);
	}
}
void ContactListProxyModel::setStatus(const QModelIndex &source, const QIcon &/*icon*/, const QString &/*text*/, int mass)
{
	removeItem(source);
	insertItem(source);
}
	
void ContactListProxyModel::checkItem(const QModelIndex &source)
{	
	if(!source.isValid())
		return;
	TreeItem *source_item = static_cast<TreeItem*>(source.internalPointer());
	int mass = source_item->data(Qt::UserRole+1).toInt();
	int visibility = source_item->getItemVisibility();
	int attributes = source_item->getAttributes();
	const TreeModelItem &structure = source_item->getStructure();
	int source_type = structure.m_item_type;
	bool show = false;
	if( source_type == 0 )
	{
		show |= (mass == 1000) && (visibility & ShowOffline || m_show_offline);
		show |= (mass < 1000)  && (visibility & ShowOnline);
		show |= (attributes & ItemHasUnreaded)		&& (visibility & ShowMessage);
		show |= (attributes & ItemHasChangedStatus)	&& (visibility & ShowStatus);
		show |= (attributes & ItemIsTyping)			&& (visibility & ShowTyping);
	}
	else
		show = true;
	bool exists = m_source_list.contains(source_item);
	if((exists && show) || !(exists || show))
		return;
	if(show)
		insertItem(source);
	else
		removeItem(source);
//	if(exists && source_item->hasContent())
//		return;
//	if(!(exists || source_item->hasContent()))
//		return;
//	if(!exists)
//	{
//		insertItem(source);
//		return;
//	}
//	int source_type = source_item->data(Qt::UserRole).toInt();
//	int mass;
//	mass = source_item->data(Qt::UserRole+1).toInt();
//	if( !m_show_offline && source_type==0 && (mass==1000))
//	{
//		if(!source_item->getAlwaysVisible())
//		{
//			removeItem(source);
//			return;
//		}
//	}
//	if(source_type==0)
//	{
//		if(source_item->getAlwaysInvisible())
//		{
//			removeItem(source);
//			return;
//		}
////		if(source_item->parent()->getAlwaysInvisible())
////		{
////			removeItem(source);
////			return;
////		}
//	}
////	else if(source_type==1)
////	{
////		if(source_item->getAlwaysInvisible())
////		{
////			removeItem(source);
////			return;
////		}
////	}
}
	
int ContactListProxyModel::findPosition(const QModelIndex &parent, const QString &name, int mass)
{
	ProxyModelItem *item = getItem(parent);
	const QList<ProxyModelItem*> *children = item->getChildren();
	m_position_buddy->setMass(mass);
	m_position_buddy->setName(name);
	int position = qLowerBound(children->begin(),children->end(),m_position_buddy,compareItem)-children->begin();
	return position;
}

void ContactListProxyModel::removeAllItems()
{
	reset();
	m_append_to_expand_list = true;
	m_tree_view->setModel(m_empty_model);
//	emit layoutAboutToBeChanged();
	m_list_for_expand.clear();
//	reset();
	m_source_list.clear();
	m_root_item->removeChildren();
//	removeRows(0,m_root_item->childCount(),QModelIndex());
	m_root_item->setSeparator(0,false);
	m_root_item->setSeparator(1,false);
	m_root_item->setSeparator(2,false);
//	emit layoutChanged();
	m_tree_model->reinsertAllItems();
	reset();
	m_tree_view->setModel(this);
	foreach(ProxyModelItem *item, m_list_for_expand)
		m_tree_view->setExpanded(createIndex_(item), true);
	m_append_to_expand_list = false;
}
void ContactListProxyModel::setShowOffline(bool show)
{
	if(show!=m_show_offline)
	{
		m_show_offline=show;
		removeAllItems();
	}
}
void ContactListProxyModel::setShowEmptyGroup(bool show)
{
	if(show!=m_show_empty_group)
	{
		m_show_empty_group=show;
		removeAllItems();
	}
}
void ContactListProxyModel::setSortStatus(bool sort)
{
	if(sort!=m_sort_status)
	{
		m_sort_status=sort;
                removeAllItems();
	}
}
void ContactListProxyModel::setShowSeparator(bool show)
{
	if(show!=m_show_separator)
	{
		m_show_separator=show;
                removeAllItems();
	}
}
void ContactListProxyModel::setModelType(int type)
{
	type%=4;
	if(type!=m_model_type)
	{
		m_model_type=type;
                removeAllItems();
	}
}
void ContactListProxyModel::newData(const QModelIndex &left, const QModelIndex &right)
{
	QModelIndex proxy_left = mapFromSource(left);
	QModelIndex proxy_right = mapFromSource(right);
	if(proxy_left.isValid() && proxy_right.isValid())
		emit dataChanged(proxy_left,proxy_right);
}
bool ContactListProxyModel::getShowOffline()
{
	return m_show_offline;
}
bool ContactListProxyModel::getShowEmptyGroup()
{
	return m_show_empty_group;
}
bool ContactListProxyModel::getSortStatus()
{
	return m_sort_status;
}
bool ContactListProxyModel::getShowSeparator()
{
	return m_show_separator;
}
int ContactListProxyModel::getModelType()
{
	return m_model_type;
}
void ContactListProxyModel::setTreeView(QTreeView *tree_view)
{
	m_tree_view=tree_view;
	connect( m_tree_view, SIGNAL(collapsed(QModelIndex)), SLOT(collapsed(QModelIndex)) );
	connect( m_tree_view, SIGNAL(expanded(QModelIndex)), SLOT(expanded(QModelIndex)) );
}
int ContactListProxyModel::getChildPosition(const QString &child)
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
void ContactListProxyModel::moveChild(const QString &child, int position)
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
void ContactListProxyModel::setChildOrder(const QStringList &order)
{
	m_child_order=order;
}
QStringList ContactListProxyModel::getChildOrder()
{
	return m_child_order;
}
void ContactListProxyModel::setExpanded(const QString & child, bool expanded)
{
	if(m_model_type==0 || m_model_type==2)
		return;
	if(!expanded)
		m_is_expanded.removeAll(child);
	else if(m_is_expanded.indexOf(child)<0)
		m_is_expanded.append(child);
}
bool ContactListProxyModel::isExpanded(const QString & child)
{
	return !(m_is_expanded.indexOf(child)<0);
}
void ContactListProxyModel::setExpanded(const QModelIndex & index, bool expanded)
{
	if(m_model_type==0 || m_model_type==2)
	{
		QModelIndex source_index = static_cast<ProxyModelItem*>(index.internalPointer())->getSourceIndex();
		if(!source_index.isValid())
			return;
		TreeItem *item = static_cast<TreeItem*>(source_index.internalPointer());
		item->setExpanded(expanded);
	}
	else
	{
		setExpanded(getItem(index)->getName(),expanded);
	}
}

QModelIndex ContactListProxyModel::createIndex_(ProxyModelItem *item)
{
	if(!item)
		return QModelIndex();
	if(item == m_root_item)
		return QModelIndex();
	/*if(item == m_root_item)
		return createIndex(0,0,item);*/
	return createIndex(item->childNumber(),0,item);
}

void ContactListProxyModel::collapsed( const QModelIndex &index )
{
	setExpanded( index, false );
}

void ContactListProxyModel::expanded( const QModelIndex &index )
{
	setExpanded( index, true );
}

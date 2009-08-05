/*
    Conference Contact List

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

#include "confcontactlist.h"

#include <QEvent>
#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include "tempglobalinstance.h"

ConfContactListEventEater::ConfContactListEventEater() : QObject(0)
{
    m_contact_list=0;
}

bool ConfContactListEventEater::eventFilter(QObject *obj, QEvent *event)
{
    if(!m_contact_list)
		return QObject::eventFilter(obj, event);
	if(event->type() == QEvent::ContextMenu)
	{
		QContextMenuEvent *menu_event = static_cast<QContextMenuEvent *>(event);
		QListView *list_view = dynamic_cast<QListView *>(obj->parent());
		if(!list_view)
			return QObject::eventFilter(obj, event);
		QModelIndex index = list_view->indexAt(menu_event->pos());
		if(index.isValid())
			m_contact_list->sendEventClicked(index,menu_event->globalPos());
	}
	if(event->type() == QEvent::MouseButtonDblClick)
	{
	    QMouseEvent *mouse_event = static_cast<QMouseEvent *>(event);
		QListView *list_view = dynamic_cast<QListView *>(obj->parent());
		if(!list_view)
			return QObject::eventFilter(obj, event);
		QModelIndex index = list_view->indexAt(mouse_event->pos());
		if(index.isValid())
		{
		    m_contact_list->sendEventActivated(index);
		}
	}
	if(event->type() == QEvent::KeyPress)
	{
		QKeyEvent *key_event = static_cast<QKeyEvent *>(event);
		if(!key_event->isAutoRepeat())
		{
			if(key_event->key() == Qt::Key_Enter || key_event->key() == Qt::Key_Return)
			{
				QListView *list_view = dynamic_cast<QListView *>(obj);
				if(!list_view)
					return QObject::eventFilter(obj, event);
				QModelIndexList list = list_view->selectionModel()->selectedIndexes();
				foreach(QModelIndex index, list)
				{
					m_contact_list->sendEventActivated(index);
				}
			}
		}
	}
	return QObject::eventFilter(obj, event);
}

void ConfContactListEventEater::itemActivated(const QModelIndex & index)
{
	m_contact_list->sendEventActivated(index);
	//AbstractContactList::instance().sendEventActivated(index);
}



ConfContactList::ConfContactList(const QString &protocol_name, const QString &conference_name, const QString &account_name, QListView *list_view)
: m_protocol_name(protocol_name), m_conference_name(conference_name),
	m_account_name(account_name)//, m_plugin_system(PluginSystem::instance())
{
	m_list_view=list_view;
	m_item_model = new ConferenceItemModel(this);
}

ConfContactList::~ConfContactList()
{
	delete m_item_model;
}

void ConfContactList::addConferenceItem(const QString &nickname)
{
	m_item_model->addBuddy(nickname);
}

void ConfContactList::removeConferenceItem(const QString &nickname)
{
	m_item_model->removeBuddy(nickname);
}

void ConfContactList::renameConferenceItem(const QString &nickname, const QString &new_nickname)
{
	m_item_model->renameBuddy(nickname,new_nickname);
}

void ConfContactList::setConferenceItemStatus(const QString &nickname, const QIcon &icon, const QString &status, int mass)
{
	m_item_model->setItemStatus(nickname,icon,status,mass);
}

void ConfContactList::setConferenceItemIcon(const QString &nickname,	const QIcon &icon, int position)
{
	m_item_model->setItemIcon(nickname,icon,position);
}

void ConfContactList::setConferenceItemRole(const QString &nickname, const QIcon &icon, const QString &role, int mass)
{
	m_item_model->setItemRole(nickname,icon,role,mass);
}

QStringList ConfContactList::getUsers()
{
	return m_item_model->getUsers();
}

void ConfContactList::sendEventActivated(const QModelIndex & index)
{
	qDebug()<<"activated";
    if(!index.isValid())
			return;
	ConferenceItem *item = static_cast<ConferenceItem *>(index.internalPointer());
	QString nickname = item->data(Qt::DisplayRole).toString();
	//PluginSystem::instance().conferenceItemActivated(m_protocol_name,m_conference_name,m_account_name,nickname);
	TreeModelItem item_struct;
	item_struct.m_protocol_name = m_protocol_name;
	item_struct.m_account_name = m_account_name;
	item_struct.m_parent_name = m_conference_name;
	item_struct.m_item_name = m_conference_name+"/"+nickname;
	item_struct.m_item_type = 33;
	TempGlobalInstance::instance().createChat(item_struct);
}

void ConfContactList::sendEventClicked(const QModelIndex & index, const QPoint & point)
{
	if(!index.isValid())
			return;
	ConferenceItem *item = static_cast<ConferenceItem *>(index.internalPointer());
	QString nickname = item->data(Qt::DisplayRole).toString();	
	TempGlobalInstance::instance().conferenceItemContextMenu(m_protocol_name, m_conference_name, m_account_name, nickname, point);
}

QString ConfContactList::getToolTip(const QString &nickname)
{
	return TempGlobalInstance::instance().getConferenceItemToolTip(m_protocol_name, m_conference_name, m_account_name, nickname);
}

void ConfContactList::nowActive()
{
	m_list_view->setModel(m_item_model);
}

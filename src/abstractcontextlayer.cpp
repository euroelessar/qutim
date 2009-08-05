/*
    AbstractContextLayer

    Copyright (c) 2008 by Rustam Chakin <qutim.develop@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#include "abstractcontextlayer.h"
#include "iconmanager.h"
//#include "abstractchatlayer.h"
//#include "abstracthistorylayer.h"
#include "abstractlayer.h"

AbstractContextLayer::AbstractContextLayer()
{
    m_open_chat_action = 0;
    m_open_hisory_action = 0;
    m_open_info_action = 0;
    m_open_conference_chat_action = 0;
//  m_open_conference_hisory_acti = 0;
    m_open_conference_info_action = 0;
    m_contact_menu = PluginSystem::instance().registerEventHandler("Core/ContactList/ContextMenu", this);
    m_conference_menu = PluginSystem::instance().registerEventHandler("Core/Conference/ContactContextMenu", this);
}

AbstractContextLayer::~AbstractContextLayer()
{
	delete m_open_chat_action;
	delete m_open_hisory_action;
	delete m_open_info_action;
        delete m_open_conference_chat_action;
//	delete m_open_conference_hisory_action;
        delete m_open_conference_info_action;
}

AbstractContextLayer &AbstractContextLayer::instance()
{
	static AbstractContextLayer acl;
	return acl;
}

void AbstractContextLayer::processEvent(Event &event)
{
    if(event.id == m_contact_menu)
    {
        itemContextMenu(event.at<TreeModelItem>(0), event.at<QPoint>(1));
    }
    else if(event.id == m_conference_menu)
    {
        conferenceItemContextMenu(event.at<QString>(0), event.at<QString>(1), event.at<QString>(2),
                                  event.at<QString>(3), event.at<QPoint>(4));
    }
}

void AbstractContextLayer::itemContextMenu(const TreeModelItem &item, const QPoint &menu_point)
{
	m_current_item = item;
	PluginSystem::instance().itemContextMenu(m_actions_list, item, menu_point);
}

void AbstractContextLayer::conferenceItemContextMenu(const QString &protocol_name,
		const QString &conference_name, const QString &account_name, const QString &nickname, const QPoint &menu_point)
{
	m_current_protocol = protocol_name;
	m_current_account = account_name;
	m_current_conference = conference_name;
	m_current_nickname = nickname;
	PluginSystem::instance().conferenceItemContextMenu(m_conference_actions_list, protocol_name, conference_name, account_name, nickname, menu_point);
}

void AbstractContextLayer::openChatWindowAction()
{
    AbstractLayer::instance().Chat()->createChat(m_current_item);
}

void AbstractContextLayer::openHistoryAction()
{
	AbstractLayer::History()->openWindow(m_current_item);
}

void AbstractContextLayer::openInformationAction()
{
	PluginSystem::instance().showContactInformation(m_current_item);
}

void AbstractContextLayer::openConferenceChatWindowAction()
{
	PluginSystem::instance().conferenceItemActivated(m_current_protocol,m_current_conference,m_current_account,m_current_nickname);
/*	TreeModelItem item;
	item.m_protocol_name = m_current_protocol;
	item.m_account_name = m_current_account;
	item.m_item_name = m_current_conference+"/"+m_current_nickname;
	item.m_item_type = 0;
	AbstractChatLayer::instance().createChat(item);*/
}

//void AbstractContextLayer::openConferenceHistoryAction()
//{
//	
//}
void AbstractContextLayer::openConferenceInformationAction()
{
	PluginSystem::instance().showConferenceContactInformation(m_current_protocol, m_current_conference, m_current_account, m_current_nickname);
}

void AbstractContextLayer::registerContactMenuAction(QAction *plugin_action)
{
	m_actions_list.append(plugin_action);
}

void AbstractContextLayer::createActions()
{
    m_actions_list.removeAll(m_open_chat_action);
    m_actions_list.removeAll(m_open_hisory_action);
    m_actions_list.removeAll(m_open_info_action);
    m_actions_list.removeAll(m_open_conference_chat_action);
//  m_actions_list.removeAll(m_open_conference_hisory_action);
    m_actions_list.removeAll(m_open_conference_info_action);
    delete m_open_chat_action;
    delete m_open_hisory_action;
    delete m_open_info_action;
    delete m_open_conference_chat_action;
//  delete m_open_conference_hisory_action;
    delete m_open_conference_info_action;

    IconManager &im = IconManager::instance();
    m_open_chat_action = new QAction(im.getIcon("message"),
                  tr("Send message"), 0);
    m_open_hisory_action = new QAction(im.getIcon("history"),
                                       tr("Message history"), 0);
    m_open_info_action = new QAction(im.getIcon("contactinfo"),
                                       tr("Contact details"), 0);
    connect(m_open_chat_action, SIGNAL(triggered()), this, SLOT(openChatWindowAction()));
    connect(m_open_hisory_action, SIGNAL(triggered()), this, SLOT(openHistoryAction()));
    connect(m_open_info_action, SIGNAL(triggered()), this, SLOT(openInformationAction()));

    m_actions_list.prepend(m_open_info_action);
    m_actions_list.prepend(m_open_hisory_action);
    m_actions_list.prepend(m_open_chat_action);

    m_open_conference_chat_action = new QAction(im.getIcon("message"),
                                                tr("Send message"), 0);
//  m_open_conference_hisory_action = new QAction(im.getIcon("history"),
//                                              tr("Message history"), 0);
    m_open_conference_info_action = new QAction(im.getIcon("contactinfo"),
                                                tr("Contact details"), 0);
    connect(m_open_conference_chat_action, SIGNAL(triggered()), this, SLOT(openConferenceChatWindowAction()));
//  connect(m_open_conference_hisory_action, SIGNAL(triggered()), this, SLOT(openConferenceHistoryAction()));
    connect(m_open_conference_info_action, SIGNAL(triggered()), this, SLOT(openConferenceInformationAction()));

    m_conference_actions_list.prepend(m_open_conference_info_action);
//  m_conference_actions_list.prepend(m_open_conference_hisory_action);
    m_conference_actions_list.prepend(m_open_conference_chat_action);
}

/*
    VpluginSystem

    Copyright (c) 2009 by Rustam Chakin <qutim.develop@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#include "vpluginsystem.h"



VpluginSystem::VpluginSystem()
{
}

VpluginSystem &VpluginSystem::instance()
{
        static VpluginSystem vps;
        return vps;
}

void VpluginSystem::setPluginSystemPointer(PluginSystemInterface *ps)
{
    m_plugin_system = ps;
}

QIcon VpluginSystem::getSystemIcon(const QString &icon_name)
{
    return m_plugin_system->getIcon(icon_name);
}

void VpluginSystem::updateStatusIcons()
{
	m_plugin_system->updateStatusIcons();
}

bool VpluginSystem::removeItemFromContactList(TreeModelItem Item)
{
	return m_plugin_system->removeItemFromContactList(Item);
}

bool VpluginSystem::moveItemInContactList(TreeModelItem OldItem, TreeModelItem NewItem)
{
	return m_plugin_system->moveItemInContactList(OldItem, NewItem);
}

bool VpluginSystem::setContactItemStatus(TreeModelItem Item, QIcon icon, QString text, int mass)
{
	return m_plugin_system->setContactItemStatus(Item, icon, text, mass);
}

void VpluginSystem::sendSystemMessage(const QString &account_name, const QString &message)
{
    TreeModelItem item;
    item.m_protocol_name = "VKontakte";
    item.m_account_name = account_name;
    item.m_item_name = account_name;
    item.m_item_type = 2;
    m_plugin_system->systemNotifiacation(item ,message);
}

void VpluginSystem::setAccountIsOnline(const QString &account_name, bool online)
{
    TreeModelItem item;
    item.m_protocol_name = "VKontakte";
    item.m_account_name = account_name;
    item.m_item_name = account_name;
    item.m_item_type = 2;
    m_plugin_system->setAccountIsOnline(item, online);
}

bool VpluginSystem::addItemToContactList(TreeModelItem Item, QString name)
{
	return m_plugin_system->addItemToContactList(Item, name);
}

bool VpluginSystem::setContactItemIcon(TreeModelItem Item, QIcon icon, int position)
{
    return m_plugin_system->setContactItemIcon(Item, icon, position);
}

bool VpluginSystem::setContactItemRow(TreeModelItem Item, QList<QVariant> row, int position)
{
	return m_plugin_system->setContactItemRow(Item, row, position);
}

void VpluginSystem::addMessageFromContact(const TreeModelItem &item, const QString &message,
		const QDateTime &message_date)
{
	m_plugin_system->addMessageFromContact(item, message, message_date);
}

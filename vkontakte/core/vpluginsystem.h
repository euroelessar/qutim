/*
    VpluginSystem

    Copyright (c) 2009 by Rustam Chakin <qutim.develop@gmail.com>
				  2009 by Alexander Kazarin <boiler@co.ru>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#ifndef VPLUGINSYSTEM_H
#define VPLUGINSYSTEM_H

#include <qutim/protocolinterface.h>
using namespace qutim_sdk_0_2;

class VpluginSystem
{
public:
    VpluginSystem();
    static VpluginSystem &instance();
    void setPluginSystemPointer(PluginSystemInterface *ps);
    QIcon getSystemIcon(const QString &icon_name);
    void updateStatusIcons();
    void sendSystemMessage(const QString &account_name, const QString &message);
    void setAccountIsOnline(const QString &account_name, bool online);
    bool addItemToContactList(TreeModelItem Item, QString name=QString());
    bool removeItemFromContactList(TreeModelItem Item);
	bool moveItemInContactList(TreeModelItem OldItem, TreeModelItem NewItem);
    //Function for settings contact status, text - status name for adium icons packs,
    //mass status mass, 1000 - offline, 999-offline separator, -1 - online separator
    bool setContactItemStatus(TreeModelItem Item, QIcon icon, QString text, int mass);
    //Function for adding icon to contact (1,2 - left side; 3...12 - right side)
    bool setContactItemIcon(TreeModelItem Item, QIcon icon, int position);
    //Function for adding text row below contact ( 1..3)
    bool setContactItemRow(TreeModelItem Item, QList<QVariant> row, int position);

    void addMessageFromContact(const TreeModelItem &item, const QString &message,
			const QDateTime &message_date);

private:
    PluginSystemInterface *m_plugin_system;
};

#endif // VPLUGINSYSTEM_H

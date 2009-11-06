/*
    VcontactList

    Copyright (c) 2009 by Alexander Kazarin <boiler@co.ru>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#include "vcontactlist.h"
#include <QSettings>
#include <QDesktopServices>

VcontactList::VcontactList(const QString &account_name, const QString &profile_name,
		 VprotocolWrap *protocol_wrap)
: m_account_name(account_name),
m_profile_name(profile_name),
m_protocol_wrap(protocol_wrap),
m_plugin_system(VpluginSystem::instance())
{

    zeroHash = QCryptographicHash::hash("0", QCryptographicHash::Md5);
    emptyHash = QCryptographicHash::hash("", QCryptographicHash::Md5);

    m_online_icon = QIcon(":/images/online.png");
    m_offline_icon = QIcon(":/images/offline.png");

    TreeModelItem item;
    item.m_protocol_name = "VKontakte";
    item.m_account_name = m_account_name;
    item.m_item_name = m_account_name;
    item.m_item_type = 2;
    m_plugin_system.addItemToContactList(item, m_account_name);

    item.m_protocol_name = "VKontakte";
    item.m_account_name = m_account_name;
    item.m_parent_name = m_account_name;
	item.m_item_name = "2";
    item.m_item_type = 1;
    m_plugin_system.addItemToContactList(item, tr("Friends"));
	loadSettings();
	QHash<QString,Buddy> tmp;
    loadLocalList(tmp);
    connect(m_protocol_wrap, SIGNAL(friendListArrived(QList<FriendBuddy>&)), this, SLOT(getFriendsList(QList<FriendBuddy>&)));
	connect(m_protocol_wrap, SIGNAL(faveListArrived(QList<FriendBuddy>&)), this, SLOT(faveListArrived(QList<FriendBuddy>&)));
    connect(m_protocol_wrap, SIGNAL(activitiesListArrived(QList<Activity>&)), this, SLOT(activitiesArrived(QList<Activity>&)));
    connect(m_protocol_wrap, SIGNAL(getNewMessages(QList<Message>&)), this, SLOT(getNewMessages(QList<Message>&)));
    m_avatar_management = new VavatarManagement(m_account_name, m_profile_name);
    connect(m_avatar_management, SIGNAL(avatarDownloaded(const QString &, const QByteArray &)),
	    this , SLOT(avatarArrived(const QString&,const QByteArray &)));
    createContactListActions();
}

VcontactList::~VcontactList()
{
    qDeleteAll(m_current_friend_list);
}

void VcontactList::removeCL()
{
    TreeModelItem item;
    item.m_protocol_name = "VKontakte";
    item.m_account_name = m_account_name;
    item.m_item_name = m_account_name;
    item.m_item_type = 2;

    m_plugin_system.removeItemFromContactList(item);
}

void VcontactList::getFriendsList(QList<FriendBuddy> &friends)
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "qutim/qutim."+m_profile_name+"/vkontakte."+m_account_name, "contactlist");

    QStringList tmp_remove_buddies = m_tmp_friend_list;
    QStringList tmp_add_buddies = m_tmp_friend_list;
    QStringList friend_list;

    QDir dir(settings.fileName().section('/', 0, -3) + "/vkontakteicons");
	if ( !dir.exists() )
	    dir.mkdir(dir.path());

    //add new users and make list for removing
	foreach(FriendBuddy mine_friend, friends) {

	    QFile file(dir.path() + "/" + QCryptographicHash::hash(mine_friend.m_avatar_url.toUtf8(),
						       QCryptographicHash::Md5).toHex());

		if ( m_current_friend_list.contains(mine_friend.m_id)) {
			if ( m_current_friend_list.value(mine_friend.m_id)->m_avatar_hash !=
					QCryptographicHash::hash(mine_friend.m_avatar_url.toUtf8(), QCryptographicHash::Md5)
			    || !file.exists())
			m_avatar_management->requestForAvatar(mine_friend.m_id, mine_friend.m_avatar_url);
		}
		else m_avatar_management->requestForAvatar(mine_friend.m_id, mine_friend.m_avatar_url);

		friend_list<<mine_friend.m_id;
        tmp_remove_buddies.removeAll(mine_friend.m_id);
		if (!tmp_add_buddies.contains(mine_friend.m_id)) {
            Buddy *tmp_buddy = new Buddy;
            tmp_buddy->m_id = mine_friend.m_id;
            tmp_buddy->m_name = mine_friend.m_name;
            tmp_buddy->m_online = false;
            m_current_friend_list.insert(mine_friend.m_id, tmp_buddy);

            settings.beginGroup(mine_friend.m_id);
            settings.setValue("id",mine_friend.m_id);
            settings.setValue("name", mine_friend.m_name);
            settings.endGroup();

            TreeModelItem item;
            item.m_protocol_name = "VKontakte";
            item.m_account_name = m_account_name;
			item.m_parent_name = getParentForId(mine_friend.m_id);
            item.m_item_name = mine_friend.m_id;
            item.m_item_type = 0;
            m_plugin_system.addItemToContactList(item, mine_friend.m_name);
            setBuddyOffline(mine_friend.m_id);

        }
    }

    foreach(QString tmp_id, tmp_remove_buddies)
    {
        if ( m_current_friend_list.contains(tmp_id) )
        {
            delete m_current_friend_list.value(tmp_id);
            m_current_friend_list.remove(tmp_id);
            settings.remove(tmp_id);
        }
    }

    settings.setValue("list/all",friend_list);

    setStatuses(friends);

}

void VcontactList::faveListArrived(QList<FriendBuddy> &faves)
{

	if (!m_current_fave_list.count()) {
		TreeModelItem item;
		item.m_protocol_name = "VKontakte";
		item.m_account_name = m_account_name;
		item.m_parent_name = m_account_name;
		item.m_item_name = "1";
		item.m_item_type = 1;
		m_plugin_system.addItemToContactList(item, tr("Favorites"));
	}

	foreach(FriendBuddy mine_fave, faves) {
		if (m_current_friend_list.contains(mine_fave.m_id)) {
			if (!m_current_fave_list.contains(mine_fave.m_id)) {

				TreeModelItem item;
				item.m_protocol_name = "VKontakte";
				item.m_account_name = m_account_name;
				item.m_item_name = mine_fave.m_id;
				item.m_item_type = 0;
				item.m_parent_name="2";

				TreeModelItem newitem = item;
				newitem.m_parent_name="1";

				m_plugin_system.moveItemInContactList(item, newitem);

				Buddy *tmp_buddy = new Buddy;
				tmp_buddy->m_id = mine_fave.m_id;
				tmp_buddy->m_name = mine_fave.m_name;
				m_current_fave_list.insert(mine_fave.m_id, tmp_buddy);
			}
		}
		else {
			if (!m_current_fave_list.contains(mine_fave.m_id)) {

				TreeModelItem item;
				item.m_protocol_name = "VKontakte";
				item.m_account_name = m_account_name;
				item.m_item_name = mine_fave.m_id;
				item.m_item_type = 0;
				item.m_parent_name="1";

				m_plugin_system.addItemToContactList(item, mine_fave.m_name);

				Buddy *tmp_buddy = new Buddy;
				tmp_buddy->m_id = mine_fave.m_id;
				tmp_buddy->m_name = mine_fave.m_name;
				m_current_fave_list.insert(mine_fave.m_id, tmp_buddy);
			}
		}
	}
}

QString VcontactList::getParentForId(QString v_id) {
	QString v_parent = "2";
	if (m_current_fave_list.contains(v_id)) v_parent = "1";
	return v_parent;
}

void VcontactList::loadLocalList(QHash<QString,Buddy> local_status_list,bool add_to_gui)
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "qutim/qutim."+m_profile_name+"/vkontakte."+m_account_name, "contactlist");
    m_avatar_path = settings.fileName().section('/', 0, -3) + "/vkontakteicons/";
   
    QStringList friend_list = settings.value("list/all", QStringList()).toStringList();
    qDeleteAll(m_current_friend_list);
    m_tmp_friend_list.clear();
    m_current_friend_list.clear();
    foreach(QString mine_friend, friend_list)
    {
	settings.beginGroup(mine_friend);
	if ( !settings.value("id","").toString().isEmpty())
	{
            QString tmp_id = settings.value("id","").toString();
            Buddy *tmp_buddy = new Buddy;
            tmp_buddy->m_id = tmp_id;
            tmp_buddy->m_name = settings.value("name","").toString();
            tmp_buddy->m_online = false;
	    tmp_buddy->m_avatar_hash = settings.value("avatar",QByteArray()).toByteArray();

            m_tmp_friend_list<<tmp_id;
            if ( local_status_list.contains(tmp_id) )
            {
                tmp_buddy->m_online = local_status_list.value(tmp_id).m_online;
            }
            m_current_friend_list.insert(tmp_id, tmp_buddy);
	    TreeModelItem item;
	    item.m_protocol_name = "VKontakte";
	    item.m_account_name = m_account_name;
	    item.m_item_name = settings.value("id","").toString();
		item.m_parent_name = getParentForId(item.m_item_name);
	    item.m_item_type = 0;
	    if (add_to_gui)
	    {
		m_plugin_system.addItemToContactList(item, settings.value("name","").toString());
		setBuddyOffline(settings.value("id","").toString());
	    }

	    if ( !tmp_buddy->m_avatar_hash.isEmpty() )
	    {
			m_plugin_system.setContactItemIcon(item,
			QIcon(settings.fileName().section('/', 0, -3) + "/vkontakteicons/" + settings.value("avatar",QByteArray()).toByteArray().toHex()), 1);

	    }

	}
	settings.endGroup();
    }
}

void VcontactList::setBuddyOffline(const QString &account_id)
{
	    TreeModelItem item;
	    item.m_protocol_name = "VKontakte";
	    item.m_account_name = m_account_name;
		item.m_parent_name = getParentForId(item.m_item_name);
	    item.m_item_name = account_id;
	    item.m_item_type = 0;
	    m_plugin_system.setContactItemStatus(item,m_offline_icon,"offline", 1000);
}

void VcontactList::setBuddyOnline(const QString &account_id)
{
	    TreeModelItem item;
	    item.m_protocol_name = "VKontakte";
	    item.m_account_name = m_account_name;
		item.m_parent_name = getParentForId(item.m_item_name);
	    item.m_item_name = account_id;
	    item.m_item_type = 0;
	    m_plugin_system.setContactItemStatus(item,m_online_icon,"online", 0);
}

void VcontactList::setAllOffline()
{
    foreach(Buddy *tmp_buddy, m_current_friend_list)
    {
        setBuddyOffline(tmp_buddy->m_id);
		tmp_buddy->m_online = false;
    }
}

void VcontactList::setStatuses(QList<FriendBuddy> &friends)
{
    m_tmp_friend_list.clear();
    foreach(FriendBuddy mine_friend, friends)
    {
        m_tmp_friend_list<<mine_friend.m_id;
        if ( m_current_friend_list.contains(mine_friend.m_id))
	{

            if (mine_friend.m_online != m_current_friend_list.value(mine_friend.m_id)->m_online)
	    {
		if ( mine_friend.m_online)
		    setBuddyOnline(mine_friend.m_id);
		else
		    setBuddyOffline(mine_friend.m_id);
                m_current_friend_list.value(mine_friend.m_id)->m_online = mine_friend.m_online;

	    }
	}
    }
}

void VcontactList::avatarArrived(const QString &buddy_id, const QByteArray &avatar_hash)
{
    if ( m_current_friend_list.contains(buddy_id) )
    {
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "qutim/qutim."+m_profile_name+"/vkontakte."+m_account_name, "contactlist");
	
	if ( avatar_hash == zeroHash )
	    QFile::remove(settings.fileName().section('/', 0, -3) + "/vkontakteicons/"
			  + m_current_friend_list.value(buddy_id)->m_avatar_hash.toHex());
	
	m_current_friend_list.value(buddy_id)->m_avatar_hash = avatar_hash;
	settings.setValue(buddy_id+"/avatar",avatar_hash);

	TreeModelItem item;
	item.m_protocol_name = "VKontakte";
	item.m_account_name = m_account_name;
	item.m_parent_name = getParentForId(item.m_item_name);
	item.m_item_name = buddy_id;
	item.m_item_type = 0;

	m_plugin_system.setContactItemIcon(item,
			QIcon(settings.fileName().section('/', 0, -3) + "/vkontakteicons/" + avatar_hash.toHex()), 1);
    }
}

void replaceImproperChars(QString &txt)
{
    txt.replace("&quot;", "\"", Qt::CaseInsensitive);
    txt.replace("&lt;", "<", Qt::CaseInsensitive);
    txt.replace("&gt;", ">", Qt::CaseInsensitive);
    txt.replace("&#39;", "'", Qt::CaseInsensitive);
    txt.replace("<br>", "\n", Qt::CaseInsensitive);
    // add here

    txt.replace("&amp;", "&", Qt::CaseInsensitive); // last
}

void VcontactList::activitiesArrived(QList<Activity> &activities)
{
    foreach(Activity tmp_act, activities)
    {
	if ( m_current_friend_list.contains(tmp_act.m_id) )
	{
	    TreeModelItem item;
	    item.m_protocol_name = "VKontakte";
	    item.m_account_name = m_account_name;
		item.m_parent_name = getParentForId(item.m_item_name);
	    item.m_item_name = tmp_act.m_id;
	    item.m_item_type = 0;
	    m_current_friend_list.value(tmp_act.m_id)->m_activity = tmp_act.m_activity;
	    if (!m_show_statustext)
		    return;
	    QList<QVariant> status;
	    QString statusText = QString(tmp_act.m_activity);
	    replaceImproperChars(statusText);
	    status.append("    " + statusText);
	    m_plugin_system.setContactItemRow(item, status, 0);
	}
    }
}

QString VcontactList::getToolTip(const QString &buddy_id)
{
    QString custom_tooltip;
    if ( m_current_friend_list.contains(buddy_id) )
    {
	Buddy *tmp_buddy = m_current_friend_list.value(buddy_id);

	custom_tooltip.append("<table><tr><td>");
	if ( tmp_buddy->m_online )
	    custom_tooltip.append("<img src=':/images/online.png'/>");
	else
	    custom_tooltip.append("<img src=':/images/offline.png'/>");

	custom_tooltip.append("<b>" + Qt::escape(tmp_buddy->m_name) + "</b><br/>");
	if ( !tmp_buddy->m_activity.isEmpty() )
	    custom_tooltip.append(tr("<font size='2'><b>Status message:</b>&nbsp;%1</font").arg(tmp_buddy->m_activity));
	custom_tooltip.append("</td><td>");

	if ( !tmp_buddy->m_avatar_hash.isEmpty()
	    && tmp_buddy->m_avatar_hash != zeroHash
	    && tmp_buddy->m_avatar_hash != emptyHash )
	{
	    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "qutim/qutim."+m_profile_name+"/vkontakte."+m_account_name, "contactlist");
	    QString avatar_path = settings.fileName().section('/', 0, -3) + "/vkontakteicons/" +  tmp_buddy->m_avatar_hash.toHex();
	    if ( QFile::exists(avatar_path))
		custom_tooltip.append("<img src='" + avatar_path + "' align='right'>");
	}

	custom_tooltip.append("</td></tr></table>");
    }
    return custom_tooltip;
}

void VcontactList::getNewMessages(QList<Message> &messages)
{
    foreach(Message tmp_mess, messages)
    {
	if ( m_current_friend_list.contains(tmp_mess.m_sender_id) )
	{
	    TreeModelItem item;
	    item.m_protocol_name = "VKontakte";
	    item.m_account_name = m_account_name;
		item.m_parent_name = getParentForId(item.m_item_name);
	    item.m_item_name = tmp_mess.m_sender_id;
	    item.m_item_type = 0;

	    replaceImproperChars(tmp_mess.m_message);


	    m_plugin_system.addMessageFromContact(item,tmp_mess.m_message, tmp_mess.m_time);
	}
    }
}

QStringList VcontactList::getInfoAboutBuddy(const QString &buddy_id)
{
    QStringList info;
    if ( m_current_friend_list.contains(buddy_id) )
    {
	info<<m_current_friend_list.value(buddy_id)->m_name;
	info<<m_avatar_path + m_current_friend_list.value(buddy_id)->m_avatar_hash.toHex();

    }
    return info;
}

void VcontactList::loadSettings()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "qutim/qutim."+m_profile_name+"/vkontakte."+m_account_name, "accountsettings");
 	m_show_statustext = settings.value("roster/statustext", true).toBool();
}

void VcontactList::showItemContextMenu(const QList<QAction*> &action_list,
			const QString &item_name, int item_type, const QPoint &point_menu)
{
    if ( item_type == 1 )
	{
//		if ( groupList.contains(item_name.toUInt()))
//		{
//			showGroupMenu(groupList.value(item_name.toUInt()), menu_point);
//		}
	}
	else if ( item_type == 0)
	{
		if ( m_current_friend_list.contains(item_name) )
		{
			showBuddyMenu(action_list,m_current_friend_list.value(item_name), point_menu);
		}
	}
}

void VcontactList::openPage(const QString &id)
{
    QDesktopServices::openUrl(QUrl("http://vkontakte.ru/id" + id));
}

void VcontactList::openPageActionTriggered()
{
    openPage(currentContextBuddy->m_id);
}

void VcontactList::createContactListActions()
{
	currentContextMenu = new QMenu;

	menuLabel = new QLabel;
	menuLabel->setAlignment(Qt::AlignCenter);
	menuTitle = new QWidgetAction(this);
	menuTitle->setDefaultWidget(menuLabel);

	openPageAction = new QAction(m_plugin_system.getSystemIcon("contactinfo"), tr("Open user page"), this);
	connect(openPageAction, SIGNAL(triggered()), this, SLOT(openPageActionTriggered()));
}

void VcontactList::showBuddyMenu(const QList<QAction*> &action_list,
		Buddy *buddy, const QPoint &point)
{
    currentContextBuddy = buddy;

    currentContextMenu->clear();

    currentContextMenu->addAction(menuTitle);
    menuLabel->setText("<b>"+ buddy->m_name +"</b>");

    currentContextMenu->addAction(action_list.at(0));
    currentContextMenu->addAction(openPageAction);
    currentContextMenu->addAction(action_list.at(1));

    currentContextMenu->addSeparator();

    int action_count = action_list.count() - 3;
    if ( action_count > 0 )
    {
	for ( int i = 0; i < action_count; i++ )
	{
	    currentContextMenu->addAction(action_list.at(3 + i));
	}
    }

    currentContextMenu->popup(point);
}


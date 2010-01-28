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

#ifndef VCONTACTLIST_H
#define VCONTACTLIST_H

#include <QtGui>
#include "../protocol/vprotocolwrap.h"
#include "vpluginsystem.h"
#include "vavatarmanagement.h"

enum BuddyType {
  btFriend,
  btNotInList,
};

class Buddy {
public:
    QString m_id;
    QString m_name;
    bool m_online;
    QByteArray m_avatar_hash;
    QString m_activity;
    BuddyType m_type;
    bool m_bookmarked;
    Buddy() {
	m_type = btFriend;
	m_bookmarked = false;
    }
};


class VcontactList : public QObject
{
    Q_OBJECT
public:
    VcontactList(const QString &account_name, const QString &profile_name,
		 VprotocolWrap *protocol_wrap);
    ~VcontactList();
    void removeCL();
    QString getToolTip(const QString &buddy_id);
	QStringList getInfoAboutBuddy(const QString &buddy_id);
	void loadSettings();
    void showItemContextMenu(const QList<QAction*> &action_list,
			const QString &item_name, int item_type, const QPoint &point_menu);
    void showBuddyMenu(const QList<QAction*> &action_list, Buddy *, const QPoint &);
    void openPage(const QString &id);
    QByteArray zeroHash;
    QByteArray emptyHash;
public slots:
    void setAllOffline();
private slots:
    void getFriendsList(QList<FriendBuddy> &friends);
	void faveListArrived(QList<FriendBuddy> &friends);
    void avatarArrived(const QString &buddy_id,const QByteArray &avatar_hash);
    void activitiesArrived(QList<Activity> &activities);
    void getNewMessages(QList<Message> &messages);
    void openPageActionTriggered();


private:
    QString m_account_name;
    QString m_profile_name;
    VprotocolWrap *m_protocol_wrap;
    VpluginSystem &m_plugin_system;
    void loadLocalList(QHash<QString,Buddy> local_status_list,bool add_to_gui = true);
    QStringList m_tmp_friend_list;
    QStringList m_tmp_temporary_list;
    QHash<QString,Buddy*> m_current_friend_list;
    QStringList m_last_fave_list;
    QString getParentForId(QString);
    QIcon m_online_icon;
    QIcon m_offline_icon;
    void setBuddyOffline(const QString &account_id);
    void setBuddyOnline(const QString &account_id);
    void setStatuses(QList<FriendBuddy> &friends);
    VavatarManagement *m_avatar_management;
    QString m_avatar_path;
    bool m_show_statustext;

    void createContactListActions();
    QMenu *currentContextMenu;
    Buddy *currentContextBuddy;
    QWidgetAction *menuTitle;
    QLabel *menuLabel;
    QAction *openPageAction;

    void addTempFriend(const QString &id, const QString &name, const QString &avatar_url, const bool bookmarked);
    void moveToAnotherGroup(const QString &id, const QString &src, const QString &dest);
    void changeContactSettings(const QString &id, const QString &name, const QString &type, const bool &bookmarked);
};

#endif // VCONTACTLIST_H

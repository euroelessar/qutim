/*
   ChatLayerClass

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

#ifndef CHATLAYERCLASS_H
#define CHATLAYERCLASS_H

#include <QObject>
#include "settings/chatlayersettings.h"
#include "separatechats.h"
#include "tabbedchats.h"
#include "tempglobalinstance.h"
using namespace qutim_sdk_0_2;




class ChatLayerClass : public QObject,public ChatLayerInterface, public EventHandler
{
    Q_OBJECT
public:
    ChatLayerClass();
    virtual ~ChatLayerClass();
    virtual void createChat(const TreeModelItem &item);
    virtual void newMessageArrivedTo(const TreeModelItem &item, const QString &message,
				     const QDateTime &date, bool history=false, bool in = true);

    virtual void setItemTypingState(const TreeModelItem &item, TypingAttribute);
    virtual void newServiceMessageArriveTo(const TreeModelItem &item, const QString &message);
    virtual void messageDelievered(const TreeModelItem &item, int message_position);
    virtual void contactChangeHisStatus(const TreeModelItem &item, const QIcon &icon);
	virtual void contactChangeHisClient(const TreeModelItem &item);
    virtual void changeOwnNickNameInConference(const TreeModelItem &item, const QString &new_nickname);
    virtual void addImage(const TreeModelItem &item, const QByteArray &image_raw) ;
    QStringList getConferenceItemsList(const TreeModelItem &item);
    virtual void setConferenceTopic(const TreeModelItem &item, const QString &topic);
    virtual void addConferenceItem(const TreeModelItem &item, const QString &name="");
    virtual void renameConferenceItem(const TreeModelItem &item, const QString &new_name);
    virtual void removeConferenceItem(const TreeModelItem &item);
    virtual void setConferenceItemStatus(const TreeModelItem &item, const QIcon &icon, const QString &status, int mass);
    virtual void setConferenceItemIcon(const TreeModelItem &item, const QIcon &icon, int position);
    virtual void setConferenceItemRole(const TreeModelItem &item, const QIcon &icon, const QString &role, int mass);

    virtual bool init(PluginSystemInterface *plugin_system);
    virtual void release();
    virtual void setProfileName(const QString &profile_name);
    virtual void setLayerInterface( LayerType type, LayerInterface *linterface);

    virtual void saveLayerSettings();
    virtual QList<SettingsStructure> getLayerSettingsList();
    virtual void removeLayerSettings();

	virtual void saveGuiSettingsPressed();

	virtual void removeGuiLayerSettings() {}

    void processEvent(Event &e);
	QTextEdit *getEditField(const TreeModelItem &item);
public slots:
    void restorePreviousChats();
private:
    void createChat(const TreeModelItem &item,bool new_message);
    void loadSettings();
    PluginSystemInterface *m_plugin_system;
    QIcon m_settings_icon;
    ChatLayerSettings *m_settings_widget;
    QTreeWidgetItem *m_settings_item;
    QString m_profile_name;
    bool m_tabbed_mode;
    SeparateChats *m_separate_management;
    TabbedChats *m_tabbed_management;
    void loadGuiSettings();
    quint16 m_event_tray_clicked;
	quint16 m_event_show_hide_cl;
	quint16 m_event_change_id;
	quint16 m_event_head;
	quint16 m_event_js;
    quint16 m_event_all_plugin_loaded;
    void readAllUnreaded();
    void checkForNewMessages(const TreeModelItem &item);
    NotificationLayerInterface *m_notification_layer;
    void saveUnreadedMessage();
    void restoreUnreadedMessages();
    bool m_open_chat_on_new_message;
    bool m_dont_show_events_if_message_chat_is_open;
    bool m_show_all;
    void loadHistoryMessages(const TreeModelItem &item);
    HistoryLayerInterface *m_history_layer;
    bool m_play_sound_if_active;


};

#endif // CHATLAYERCLASS_H

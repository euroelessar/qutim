/*
    DefaultNotificationLayer

    Copyright (c) 2008 by Rustam Chakin <qutim.develop@gmail.com>
                  2009 by Nigmatullin Ruslan <euroelessar@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#ifndef DEFAULTNOTIFICATIONLAYER_H
#define DEFAULTNOTIFICATIONLAYER_H

#include "../../../include/qutim/plugininterface.h"
#include "../../../include/qutim/layerinterface.h"
#include <QString>
#include "notificationslayersettings.h"
#include "popupwindow.h"

class PopupWindow;

using namespace qutim_sdk_0_2;

struct NotificationEvents
{
	quint16 popup_press_left;
	quint16 popup_press_right;
	quint16 popup_press_middle;
	quint16 popup_release_left;
	quint16 popup_release_right;
	quint16 popup_release_middle;
	quint16 popup_dbl_click_left;
	quint16 popup_dbl_click_right;
	quint16 popup_dbl_click_middle;
	quint16 get_sound_enable;
	quint16 set_sound_enable;
	quint16 sound_enabled;
	quint16 get_popup_enable;
	quint16 set_popup_enable;
	quint16 popup_enabled;
};

class DefaultNotificationLayer : public NotificationLayerInterface, public EventHandler
{
public:
	DefaultNotificationLayer();
	virtual bool init(PluginSystemInterface *plugin_system);
    virtual void release() {}
    virtual void setProfileName(const QString &profile_name);
    void loadSettings();
    void loadTheme();
    virtual void setLayerInterface( LayerType type, LayerInterface *interface);

    virtual void saveLayerSettings();
    virtual QList<SettingsStructure> getLayerSettingsList();
    virtual void removeLayerSettings();

    virtual void saveGuiSettingsPressed() {}
//    virtual QList<SettingsStructure> getGuiSettingsList() = 0;
    virtual void removeGuiLayerSettings() {}

	virtual void showPopup(const TreeModelItem &item, const QString &message, NotificationType type);
	virtual void playSound(const TreeModelItem &item, NotificationType type);
	virtual void notify(const TreeModelItem &item, const QString &message, NotificationType type);
//	virtual void startupMessage() {}
    void deletePopupWindow(PopupWindow *window);
	NotificationEvents *getEvents() { return m_events; }
	virtual void processEvent(Event &event);
private:
    TrayLayerInterface *m_tray_layer;
	SoundEngineLayerInterface *m_sound_layer;
    QString m_profile_name;
    bool m_show_popup;
    int m_popup_width;
    int m_popup_height;
    int m_popup_sec;
    int m_popup_position;
    int m_popup_style;
    bool m_show_balloon;
    int m_balloon_sec;
    bool m_show_signon;
    bool m_show_signoff;
    bool m_show_typing;
    bool m_show_change_status;
    int m_position_in_stack;
    bool m_show_message;
    bool m_first_tray_message_is_shown;
    QList<PopupWindow *> m_popup_list;
    QString m_tray_theme_path;
    QString m_tray_theme_header_onl;
    QString m_tray_theme_header_onl_css;
    QString m_tray_theme_content_onl;
    QString m_tray_theme_content_onl_css;
    QString m_tray_theme_header_msg;
    QString m_tray_theme_header_msg_css;
    QString m_tray_theme_content_msg;
    QString m_tray_theme_content_msg_css;
    QString m_tray_theme_header_sys;
    QString m_tray_theme_header_sys_css;
    QString m_tray_theme_content_sys;
    QString m_tray_theme_content_sys_css;
	QVector<QString> m_sound_path;
	bool m_enable_sound;
	NotificationEvents *m_events;
	PluginSystemInterface *m_plugin_system;
	ChatLayerInterface *m_chat_layer;
};

#endif // DEFAULTNOTIFICATIONLAYER_H

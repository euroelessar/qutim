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

#include "defaultnotificationlayer.h"
#include "soundlayersettings.h"
#include <QTreeWidgetItem>
#include "src/iconmanager.h"

DefaultNotificationLayer::DefaultNotificationLayer()
	: m_sound_path(static_cast<int>(NotifyCount)),
	m_events(new NotificationEvents)
{
	m_settings.clear();
    m_first_tray_message_is_shown = false;
    m_position_in_stack = 1;
    m_show_message = true;
	m_tray_layer = 0;
}

bool DefaultNotificationLayer::init(PluginSystemInterface *plugin_system)
{
	m_name = "qutim";
	quint8 major, minor, secminor;
	quint16 svn;
	m_plugin_system = plugin_system;
	m_plugin_system->getQutimVersion(major, minor, secminor, svn);
	m_version = QString("%1.%2.%3 r%4").arg(major).arg(minor).arg(secminor).arg(svn);
	m_events->popup_press_left = plugin_system->registerEventHandler("Core/Notification/PopupPressedByLeft");
	m_events->popup_press_right = plugin_system->registerEventHandler("Core/Notification/PopupPressedByRight");
	m_events->popup_press_middle = plugin_system->registerEventHandler("Core/Notification/PopupPressedByMiddle");
	m_events->popup_release_left = plugin_system->registerEventHandler("Core/Notification/PopupReleasedByLeft");
	m_events->popup_release_right = plugin_system->registerEventHandler("Core/Notification/PopupReleasedByRight");
	m_events->popup_release_middle = plugin_system->registerEventHandler("Core/Notification/PopupReleasedByMiddle");
	m_events->popup_dbl_click_left = plugin_system->registerEventHandler("Core/Notification/PopupDoubleClickedByLeft");
	m_events->popup_dbl_click_right = plugin_system->registerEventHandler("Core/Notification/PopupDoubleClickedByRight");
	m_events->popup_dbl_click_middle = plugin_system->registerEventHandler("Core/Notification/PopupDoubleClickedByMiddle");
	m_events->get_sound_enable = plugin_system->registerEventHandler("Core/Notification/GetSoundIsEnabled", this);
	m_events->set_sound_enable = plugin_system->registerEventHandler("Core/Notification/SetSoundIsEnabled", this);
	m_events->sound_enabled = plugin_system->registerEventHandler("Core/Notification/SoundIsEnabled");
	m_events->get_popup_enable = plugin_system->registerEventHandler("Core/Notification/GetPopupIsEnabled", this);
	m_events->set_popup_enable = plugin_system->registerEventHandler("Core/Notification/SetPopupIsEnabled", this);
	m_events->popup_enabled = plugin_system->registerEventHandler("Core/Notification/PopupIsEnabled");
	return true;
}

void DefaultNotificationLayer::setProfileName(const QString &profile_name)
{
    m_profile_name = profile_name;
    loadSettings();
    loadTheme();
}

void DefaultNotificationLayer::loadSettings()
{
    QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
    settings.beginGroup("notifications");
    m_show_popup = settings.value("popup", true).toBool();
    m_popup_width = settings.value("pwidth", 280).toInt();
    m_popup_height = settings.value("pheight", 110).toInt();
    m_popup_sec = settings.value("ptime", 6).toInt();
    m_popup_position = settings.value("pposition", 3).toInt();
    m_popup_style = settings.value("pstyle", 1).toInt();
    m_show_balloon = settings.value("balloon", false).toBool();
    m_balloon_sec = settings.value("bsecs", 5).toInt();
    m_show_signon = settings.value("signon", true).toBool();
    m_show_signoff = settings.value("signoff", true).toBool();
    m_show_typing = settings.value("typing", true).toBool();
    m_show_change_status = settings.value("statuschange", true).toBool();
    m_show_message = settings.value("message", true).toBool();
    settings.endGroup();
	settings.beginGroup("sounds");
	m_enable_sound = settings.value("enable",true).toBool();
	for(int i=0;i<qutim_sdk_0_2::NotifyCount;i++)
	{
		QString file_name = settings.value("event"+QString::number(i), "-").toString();
		m_sound_path[i] = (file_name.at(0) == '+') ? file_name.mid(1) : "";
	}
	Event popup_event(m_events->popup_enabled, 1, &m_show_popup);
	m_plugin_system->sendEvent(popup_event);
	Event sound_event(m_events->sound_enabled, 1, &m_enable_sound);
	m_plugin_system->sendEvent(sound_event);
	settings.endGroup();
}

void DefaultNotificationLayer::loadTheme()
{
    QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
    settings.beginGroup("gui");
    m_tray_theme_path = settings.value("popup",":/style/traytheme").toString();
    settings.endGroup();
    if ( m_tray_theme_path.isEmpty() )
    {
        m_tray_theme_content_msg.clear();
        m_tray_theme_content_msg_css.clear();
        m_tray_theme_content_onl.clear();
        m_tray_theme_content_onl_css.clear();
        m_tray_theme_content_sys.clear();
        m_tray_theme_content_sys_css.clear();
        m_tray_theme_header_msg.clear();
        m_tray_theme_header_msg_css.clear();
        m_tray_theme_header_onl.clear();
        m_tray_theme_header_onl_css.clear();
        m_tray_theme_header_sys.clear();
        m_tray_theme_header_sys_css.clear();
    } else
    {
        QFile fileOnlHead(m_tray_theme_path + "/onlalert/header.html");

        if ( fileOnlHead.open(QIODevice::ReadOnly))
        {
            m_tray_theme_header_onl = fileOnlHead.readAll();
            fileOnlHead.close();

        }

        QFile fileOnlHeadCSS(m_tray_theme_path + "/onlalert/header.css");

        if ( fileOnlHeadCSS.open(QIODevice::ReadOnly))
        {
            m_tray_theme_header_onl_css = fileOnlHeadCSS.readAll();
            fileOnlHeadCSS.close();
        }

        QFile fileOnlCont(m_tray_theme_path + "/onlalert/content.html");

        if ( fileOnlCont.open(QIODevice::ReadOnly))
        {
            m_tray_theme_content_onl = fileOnlCont.readAll();
            fileOnlCont.close();
        }

        QFile fileOnlContCSS(m_tray_theme_path + "/onlalert/content.css");

        if ( fileOnlContCSS.open(QIODevice::ReadOnly))
        {
            m_tray_theme_content_onl_css = fileOnlContCSS.readAll();
            fileOnlContCSS.close();
        }

        QFile fileMsgHead(m_tray_theme_path + "/msg/header.html");

        if ( fileMsgHead.open(QIODevice::ReadOnly))
        {
            m_tray_theme_header_msg = fileMsgHead.readAll();
            fileMsgHead.close();
        }

        QFile fileMsgHeadCSS(m_tray_theme_path + "/msg/header.css");

        if ( fileMsgHeadCSS.open(QIODevice::ReadOnly))
        {
            m_tray_theme_header_msg_css = fileMsgHeadCSS.readAll();
            fileMsgHeadCSS.close();
        }


        QFile fileMsgCont(m_tray_theme_path + "/msg/content.html");

        if ( fileMsgCont.open(QIODevice::ReadOnly))
        {
            m_tray_theme_content_msg = fileMsgCont.readAll();
            fileMsgCont.close();
        }

        QFile fileMsgContCSS(m_tray_theme_path + "/msg/content.css");

        if ( fileMsgContCSS.open(QIODevice::ReadOnly))
        {
            m_tray_theme_content_msg_css = fileMsgContCSS.readAll();
            fileMsgContCSS.close();
        }

        QFile fileSysHead(m_tray_theme_path + "/system/header.html");

        if ( fileSysHead.open(QIODevice::ReadOnly))
        {
            m_tray_theme_header_sys = fileSysHead.readAll();
            fileSysHead.close();
        }

        QFile fileSysHeadCSS(m_tray_theme_path + "/system/header.css");

        if ( fileSysHeadCSS.open(QIODevice::ReadOnly))
        {
            m_tray_theme_header_sys_css = fileSysHeadCSS.readAll();
            fileSysHeadCSS.close();
        }

        QFile fileSysCont(m_tray_theme_path + "/system/content.html");

        if ( fileSysCont.open(QIODevice::ReadOnly))
        {
            m_tray_theme_content_sys = fileSysCont.readAll();
            fileSysCont.close();
        }

        QFile fileSysContCSS(m_tray_theme_path + "/system/content.css");

        if ( fileSysContCSS.open(QIODevice::ReadOnly))
        {
            m_tray_theme_content_sys_css = fileSysContCSS.readAll();
            fileSysContCSS.close();
        }
    }
}

void DefaultNotificationLayer::setLayerInterface( LayerType type, LayerInterface *interface)
{
    switch(type)
    {
    case TrayLayer:
		m_tray_layer = reinterpret_cast<TrayLayerInterface *>(interface);
        break;
	case SoundEngineLayer:
		m_sound_layer = reinterpret_cast<SoundEngineLayerInterface *>(interface);
		break;
	case ChatLayer:
		m_chat_layer = reinterpret_cast<ChatLayerInterface *>(interface);
		break;
    default:
        break;
    }
}

void DefaultNotificationLayer::saveLayerSettings()
{
    foreach(const SettingsStructure &settings, m_settings)
	{
        NotificationsLayerSettings* ns = dynamic_cast<NotificationsLayerSettings*>(settings.settings_widget);
        if(ns)
            ns->saveSettings();
        SoundLayerSettings *ss = dynamic_cast<SoundLayerSettings*>(settings.settings_widget);
        if(ss)
			ss->saveSettings();
	}
    loadSettings();
}

QList<SettingsStructure> DefaultNotificationLayer::getLayerSettingsList()
{
    SettingsStructure settings;
    settings.settings_item = new QTreeWidgetItem();
    settings.settings_item ->setText(0, QObject::tr("Notifications"));
    settings.settings_item ->setIcon(0, IconManager::instance().getIcon("events"));
    settings.settings_widget = new NotificationsLayerSettings(m_profile_name);
    m_settings.append(settings);
    settings.settings_item = new QTreeWidgetItem();
    settings.settings_item ->setText(0, QObject::tr("Sound notifications"));
    settings.settings_item ->setIcon(0, IconManager::instance().getIcon("events"));
    settings.settings_widget = new SoundLayerSettings(m_profile_name);
	m_settings.append(settings);
    return m_settings;
}

void DefaultNotificationLayer::removeLayerSettings()
{
    foreach(const SettingsStructure &settings, m_settings)
	{
		delete settings.settings_item;
		delete settings.settings_widget;
	}
    m_settings.clear();
}

void DefaultNotificationLayer::showPopup(const TreeModelItem &item, const QString &message, NotificationType type)
{
	switch( type )
	{
	case NotifySystem:
	case NotifyStatusChange:
	case NotifyMessageGet:
	case NotifyTyping:
	case NotifyBlockedMessage:
	case NotifyBirthday:
	case NotifyCustom:
	case NotifyOnline:
	case NotifyOffline:
		break;
	default:
		return;
	}
	PluginSystem::instance().userNotification(item, message, type);
	QString trayMessageMsg;
	QString msg;
	//read contact and account info
	PluginSystem &ps = PluginSystem::instance();
	QStringList contact_info;
	QStringList account_info;
	TreeModelItem account_item = item;
	account_item.m_item_name = item.m_account_name;
	QString contact_nick = item.m_item_name,
	contact_avatar,
	account_nick = item.m_account_name,
	account_avatar;
	if( type != NotifySystem )
	{
		contact_info = ps.getAdditionalInfoAboutContact(item);
		account_info = ps.getAdditionalInfoAboutContact(account_item);
		if ( contact_info.count() > 0)
		{
			contact_nick = contact_info.at(0);
		}
		if ( contact_info.count() > 1 )
		{
			contact_avatar= contact_info.at(1);
		}

		if ( account_info.count() > 0)
		{
			account_nick = account_info.at(0);
		}
		if ( account_info.count() > 1 )
		{
			account_avatar = account_info.at(1);
		}
	}
	bool show_message = false;
	switch ( type )
	{
	case NotifySystem:
		msg = message;
		show_message = true;
		break;
	case NotifyStatusChange:
		msg = contact_nick + " " + message;
		trayMessageMsg =  message;
		show_message = m_show_change_status;
		break;
	case NotifyMessageGet:
		msg = QObject::tr("Message from %1:\n%2").arg(contact_nick).arg(message);
		trayMessageMsg = message;
		show_message = m_show_message;
		break;
	case NotifyTyping:
		msg = QObject::tr("%1 is typing").arg(contact_nick);
		trayMessageMsg = QObject::tr("typing");
		show_message = m_show_typing;
		break;
	case NotifyBlockedMessage:
		msg = QObject::tr("Blocked message from %1:\n%2").arg(contact_nick).arg(message);
		trayMessageMsg = QObject::tr("(BLOCKED)\n") + message;
		show_message = true;
		break;
	case NotifyBirthday:
		msg = QObject::tr("%1 has birthday today!!").arg(contact_nick);
		trayMessageMsg = QObject::tr("has birthday today!!");
		show_message = true;
		break;
	case NotifyCustom:
		msg = contact_nick + "(" + item.m_item_name + ") " + message;
		trayMessageMsg =  message;
		show_message = true;
		break;
	case NotifyOnline:
		msg = contact_nick + " " + message;
		trayMessageMsg =  message;
		show_message = m_show_signon;
		break;
	case NotifyOffline:
		msg = contact_nick + " " + message;
		trayMessageMsg =  message;
		show_message = m_show_signoff;
		break;
		// It's impossible.. but who knows?..
	default:
		return;
//        msg = contact_nick + " " + message;
//        trayMessageMsg =  message;
//        show_message = true;
	}
	if ( m_show_balloon && show_message && m_tray_layer )
	{
		m_tray_layer->showMessage(account_nick, msg, m_balloon_sec * 1000);
	}

	if ( m_show_popup && show_message )
	{
		if ( !m_first_tray_message_is_shown )
		{
			m_first_tray_message_is_shown = true;
			m_position_in_stack = 1;
		}

		PopupWindow *popup_window = new PopupWindow(this, m_chat_layer,item, type, message,
				m_popup_width,m_popup_height, m_popup_sec, m_popup_position,
							m_popup_style, m_position_in_stack, type != NotifySystem );

		switch(type)
		{
		case NotifySystem:
			popup_window->setTheme(m_tray_theme_header_sys, m_tray_theme_header_sys_css, m_tray_theme_content_sys,
					m_tray_theme_content_sys_css, m_tray_theme_path);
			break;
		case NotifyMessageGet:
			popup_window->setTheme(m_tray_theme_header_msg, m_tray_theme_header_msg_css, m_tray_theme_content_msg,
					m_tray_theme_content_msg_css, m_tray_theme_path);
			break;
		default:
			popup_window->setTheme(m_tray_theme_header_onl, m_tray_theme_header_onl_css, m_tray_theme_content_onl,
					m_tray_theme_content_onl_css, m_tray_theme_path);
		}
		if(type == NotifySystem)
			popup_window->setSystemData(item.m_account_name, message);
		else
			popup_window->setData(item, account_nick,
					contact_nick, contact_avatar,trayMessageMsg);
		popup_window->firstTrayWindow = m_first_tray_message_is_shown;

		popup_window->showTrayMessage();

		if ( (++m_position_in_stack) > 3)
			m_position_in_stack = 3;

		m_popup_list.append(popup_window);
	}
}

void DefaultNotificationLayer::playSound(const TreeModelItem &item, NotificationType type)
{
	QString file_name = m_sound_path.value(type, QString());

	if(!file_name.isEmpty() && m_sound_layer && m_enable_sound)
		m_sound_layer->playSound(file_name);
}

void DefaultNotificationLayer::notify(const TreeModelItem &item, const QString &message, NotificationType type)
{
	playSound(item, type);
	showPopup(item, message, type);
}

void DefaultNotificationLayer::deletePopupWindow(PopupWindow *window)
{
    if ( window->firstTrayWindow )
    {
        m_first_tray_message_is_shown = false;
    }
    m_popup_list.removeAll(window);
}

void DefaultNotificationLayer::processEvent(Event &event)
{
	if(event.id == m_events->get_popup_enable)
	{
		Event ev(m_events->popup_enabled, 1, &m_show_popup);
		m_plugin_system->sendEvent(ev);
	}
	else if(event.id == m_events->get_sound_enable)
	{
		Event ev(m_events->sound_enabled, 1, &m_enable_sound);
		m_plugin_system->sendEvent(ev);
	}
	else if(event.id == m_events->set_popup_enable)
	{
		m_show_popup = event.at<bool>(0);
		QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
		settings.setValue("notifications/popup", m_show_popup);
		Event ev(m_events->popup_enabled, 1, &m_show_popup);
		m_plugin_system->sendEvent(ev);
	}
	else if(event.id == m_events->set_sound_enable)
	{
		m_enable_sound = event.at<bool>(0);
		QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
		settings.setValue("sounds/enable", m_enable_sound);
		Event ev(m_events->sound_enabled, 1, &m_enable_sound);
		m_plugin_system->sendEvent(ev);
	}
}

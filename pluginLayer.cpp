/*
    W7 integration plugin

    Copyright (c) 2009 by Belov Nikita <null@deltaz.ru>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#include <QMessageBox>
#include "pluginlayer.h"

bool w7ilayer::init( PluginSystemInterface *plugin_system )
{
	if ( QSysInfo::windowsVersion() < QSysInfo::WV_WINDOWS7 )
		return false;

	PluginInterface::init( plugin_system );
	m_plugin_system = plugin_system;
	m_icon = new QIcon();

	m_taskbar = new w7itaskbar();
	if ( !m_taskbar->init() )
		return false;

	m_event_tabbed_common_window_created = m_plugin_system->registerEventHandler( "Core/ChatWindow/TabbedCommonWindowCreated", this );
	m_event_tabbed_common_window_deleted = m_plugin_system->registerEventHandler( "Core/ChatWindow/TabbedCommonWindowDeleted", this );

	m_event_tabbed_chats_window_created = m_plugin_system->registerEventHandler( "Core/ChatWindow/TabbedChatsWindowCreated", this );
	m_event_tabbed_chats_window_deleted = m_plugin_system->registerEventHandler( "Core/ChatWindow/TabbedChatsWindowDeleted", this );

	m_event_tabbed_confs_window_created = m_plugin_system->registerEventHandler( "Core/ChatWindow/TabbedConfsWindowCreated", this );
	m_event_tabbed_confs_window_deleted = m_plugin_system->registerEventHandler( "Core/ChatWindow/TabbedConfsWindowDeleted", this );

	event_tab_alert = m_plugin_system->registerEventHandler( "Core/ChatWindow/TabAlert", this );
	event_tab_activated_by_user = m_plugin_system->registerEventHandler( "Core/ChatWindow/TabActivatedByUser", this );
	event_tab_closed = m_plugin_system->registerEventHandler( "Core/ChatWindow/TabClosed", this );

	m_tabbed_common_window_id = 0;
	m_tabbed_chats_window_id = 0;
	m_tabbed_confs_window_id = 0;

	return true;
}
void w7ilayer::release()
{
}

void w7ilayer::processEvent( Event &event )
{
	if ( event.id == m_event_tabbed_common_window_created )
	{
		m_tabbed_common_window_id = ((QWidget*)event.args.at(0))->winId();
		m_taskbar->setHWND( m_tabbed_common_window_id, 1 );
	}
	else if ( event.id == m_event_tabbed_common_window_deleted )
	{
		m_tabbed_common_window_id = 0;
		m_taskbar->setHWND( 0, 1 );
	}
	else if ( event.id == m_event_tabbed_chats_window_created )
	{
		m_tabbed_chats_window_id = ((QWidget*)event.args.at(0))->winId();
		m_taskbar->setHWND( m_tabbed_chats_window_id, 2 );
	}
	else if ( event.id == m_event_tabbed_chats_window_deleted )
	{
		m_tabbed_chats_window_id = 0;
		m_taskbar->setHWND( 0, 2 );
	}
	else if ( event.id == m_event_tabbed_confs_window_created )
	{
		m_tabbed_confs_window_id = ((QWidget*)event.args.at(0))->winId();
		m_taskbar->setHWND( m_tabbed_chats_window_id, 3 );
	}
	else if ( event.id == m_event_tabbed_confs_window_deleted )
	{
		m_tabbed_confs_window_id = 0;
		m_taskbar->setHWND( 0, 3 );
	}
	else if ( event.id == event_tab_alert )
	{
		m_taskbar->tabAlert( (TreeModelItem*)event.args.at(0), true );
	}
	else if ( event.id == event_tab_activated_by_user )
	{
		m_taskbar->tabAlert( (TreeModelItem*)event.args.at(0), false );
	}
	else if ( event.id == event_tab_closed )
	{
		m_taskbar->tabAlert( (TreeModelItem*)event.args.at(0), false );
	}
}

void w7ilayer::setProfileName( const QString &profile_name )
{
	m_profile_name = profile_name;
}

QString w7ilayer::name()
{
	return "W7 integration";
}

QString w7ilayer::description()
{
	return "Windows 7 shell integration plugin";
}

QIcon *w7ilayer::icon()
{
	return m_icon;
}

QString w7ilayer::type()
{
	return "integration";
}

void w7ilayer::saveSettings()
{
//	settingswidget->saveSettings();
}

QWidget *w7ilayer::settingsWidget()
{
	QWidget *settingswidget = new QWidget;
	return settingswidget;
}

void w7ilayer::removeSettingsWidget()
{
//	delete settingswidget;
}

Q_EXPORT_PLUGIN2( w7ilayer, w7ilayer );

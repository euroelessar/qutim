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

#ifndef W7IPLUGINLAYER_H
#define W7IPLUGINLAYER_H

#include <windows.h>

#if (WINVER < 0x0601)
#error Plugin requires Windows 7 (WINVER >= 0x0601) or newer
#endif

#include <qutim/plugininterface.h>
using namespace qutim_sdk_0_2;

#include "wTaskBar.h"
#include "wIcon.h"

class w7itaskbar;

class w7ilayer : public QObject, SimplePluginInterface, EventHandler
{
	Q_OBJECT
	Q_INTERFACES( qutim_sdk_0_2::PluginInterface )

public:
	virtual bool init( PluginSystemInterface *plugin_system );
	virtual void release();
	virtual void processEvent( Event &event );
	virtual void setProfileName( const QString &profile_name );
	virtual QString name();
	virtual QString description();
	virtual QString type();
	virtual QIcon *icon();
	virtual QWidget *settingsWidget();
	virtual void saveSettings();
	virtual void removeSettingsWidget();

private:
	PluginSystemInterface *m_plugin_system;
	QString m_profile_name;
	QString m_account_name;
	QIcon *m_icon;

	qint16 m_event_tabbed_common_window_created,
			m_event_tabbed_common_window_deleted,
			m_event_tabbed_chats_window_created,
			m_event_tabbed_chats_window_deleted,
			m_event_tabbed_confs_window_created,
			m_event_tabbed_confs_window_deleted,
			event_tab_alert,
			event_tab_activated_by_user,
			event_tab_closed;

	HWND m_tabbed_common_window_id,
			m_tabbed_chats_window_id,
			m_tabbed_confs_window_id;

	w7itaskbar *m_taskbar;
};
#endif

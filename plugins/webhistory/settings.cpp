/*
	historySettings

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

#include "settings.h"

historySettings::historySettings(QString profile_name)
{
	ui.setupUi(this);
	m_profile_name = profile_name;

	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "qutim/qutim."+m_profile_name, "plugin_webhistory");
	ui.editUrl->setText(settings.value("url").toString());
	ui.editAuthLogin->setText(settings.value("login").toString());
	ui.editAuthPassword->setText(settings.value("password").toString());
	ui.cbNotify->setChecked(settings.value("notify", true).toBool());
	ui.cbStoreJSON->setChecked(settings.value("store/json").toBool());
	ui.cbStoreSQLite->setChecked(settings.value("store/sqlite").toBool());
}

historySettings::~historySettings()
{
	
}

void historySettings::saveSettings()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "qutim/qutim."+m_profile_name, "plugin_webhistory");
	settings.setValue("url", ui.editUrl->text());
	settings.setValue("login", ui.editAuthLogin->text());
	settings.setValue("password", ui.editAuthPassword->text());
	settings.setValue("notify", ui.cbNotify->isEnabled());
	settings.setValue("store/json", ui.cbStoreJSON->isEnabled());
	settings.setValue("store/sqlite", ui.cbStoreSQLite->isEnabled());

	emit settingsChanged();
}


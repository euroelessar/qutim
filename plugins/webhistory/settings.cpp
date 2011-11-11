/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Alexander Kazarin <boiler@co.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

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


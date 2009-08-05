/*
    HistorySettings

    Copyright (c) 2008 by Rustam Chakin <qutim.develop@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#include "historysettings.h"
#include <QSettings>

HistorySettings::HistorySettings(const QString &profile_name,QWidget *parent)
    : QWidget(parent),
    m_profile_name(profile_name)
{
	ui.setupUi(this);
	changed = false;
	loadSettings();
	connect(ui.saveHistoryBox, SIGNAL(stateChanged(int)),
				this, SLOT(widgetStateChanged()));
	connect(ui.recentBox, SIGNAL(stateChanged(int)),
				this, SLOT(widgetStateChanged()));
	connect(ui.messagesCountBox, SIGNAL(valueChanged(int)),
				this, SLOT(widgetStateChanged()));
}

HistorySettings::~HistorySettings()
{

}

void HistorySettings::loadSettings()
{
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
	settings.beginGroup("history");
	ui.saveHistoryBox->setChecked(settings.value("save", true).toBool());
	ui.recentBox->setChecked(settings.value("showrecent", true).toBool());
	ui.messagesCountBox->setValue(settings.value("recentcount", 4).toUInt());
	settings.endGroup();
}

void HistorySettings::saveSettings()
{
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
	settings.beginGroup("history");
	settings.setValue("save", ui.saveHistoryBox->isChecked());
	settings.setValue("showrecent", ui.recentBox->isChecked());
	settings.setValue("recentcount", ui.messagesCountBox->value());
	settings.endGroup();
	if ( changed )
			emit settingsSaved();
	changed = false;
}

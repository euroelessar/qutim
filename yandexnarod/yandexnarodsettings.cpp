/*
    yandexnarodPluginSettings

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

#include "yandexnarodsettings.h"

yandexnarodSettings::yandexnarodSettings(QString profile_name) 
{
	ui.setupUi(this);
	m_profile_name = profile_name;
	ui.labelStatus->setText(NULL);

	ui.labelAbout->setText(ui.labelAbout->text().replace("%VERSION%", VERSION));

	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "qutim/qutim."+m_profile_name, "plugin_yandexnarod");
	ui.editLogin->setText(settings.value("auth/login").toString());
	ui.editPasswd->setText(settings.value("auth/passwd").toString());
	if (settings.value("main/msgtemplate").isValid()) {
		ui.textTpl->setText(settings.value("main/msgtemplate").toString());
	}
	else {
		ui.textTpl->setText("File sent: %N (%S bytes)\n%U");
	}

	connect(ui.btnTest, SIGNAL(clicked()), this,  SLOT(saveSettings()));
	connect(ui.btnTest, SIGNAL(clicked()), this,  SIGNAL(testclick()));
}

yandexnarodSettings::~yandexnarodSettings()
{
	
}

void yandexnarodSettings::saveSettings()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "qutim/qutim."+m_profile_name, "plugin_yandexnarod");
	settings.setValue("auth/login", ui.editLogin->text());
	settings.setValue("auth/passwd", ui.editPasswd->text());
	settings.setValue("main/msgtemplate", ui.textTpl->toPlainText());
}

void yandexnarodSettings::setStatus(QString str)
{
	ui.labelStatus->setText(str);
}

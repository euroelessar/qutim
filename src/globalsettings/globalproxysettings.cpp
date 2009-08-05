/*
    GlobalProxySettings

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

#include "globalproxysettings.h"

GlobalProxySettings::GlobalProxySettings(const QString &profile_name, QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
	m_profile_name = profile_name;
	changed = false;
	loadSettings();
	connect(ui.typeBox_2, SIGNAL(currentIndexChanged(int)),
				this, SLOT(widgetStateChanged()));
	connect(ui.proxyHostEdit_2, SIGNAL(textChanged(const QString &)),
				this, SLOT(widgetStateChanged()));
	connect(ui.proxyPortBox_2, SIGNAL(valueChanged(int)),
				this, SLOT(widgetStateChanged()));
	connect(ui.authBox_2, SIGNAL(stateChanged(int)),
				this, SLOT(widgetStateChanged()));
	connect(ui.userNameEdit_2, SIGNAL(textChanged(const QString &)),
				this, SLOT(widgetStateChanged()));
	connect(ui.userPasswordEdit_2, SIGNAL(textChanged(const QString &)),
				this, SLOT(widgetStateChanged()));
}

GlobalProxySettings::~GlobalProxySettings()
{

}

void GlobalProxySettings::loadSettings()
{
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
	settings.beginGroup("proxy");
	ui.proxyHostEdit_2->setText(settings.value("proxy/host", "").toString());
	ui.proxyPortBox_2->setValue(settings.value("proxy/port", 1).toInt());
	ui.authBox_2->setChecked(settings.value("proxy/auth", false).toBool());
	ui.userNameEdit_2->setText(settings.value("proxy/user", "").toString());
	ui.userPasswordEdit_2->setText(settings.value("proxy/pass", "").toString());
	int proxy_type = settings.value("proxy/proxyType", 0).toInt();
	ui.typeBox_2->setCurrentIndex(proxy_type);
	on_typeBox_2_currentIndexChanged(proxy_type);
	settings.endGroup();
}

void GlobalProxySettings::saveSettings()
{
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
	settings.beginGroup("proxy");
	settings.setValue("proxy/proxyType", ui.typeBox_2->currentIndex());
	settings.setValue("proxy/host", ui.proxyHostEdit_2->text());
	settings.setValue("proxy/port", ui.proxyPortBox_2->value());
	settings.setValue("proxy/auth", ui.authBox_2->isChecked());
	settings.setValue("proxy/user", ui.userNameEdit_2->text());
	settings.setValue("proxy/pass", ui.userPasswordEdit_2->text());
	settings.endGroup();
	if ( changed )
			emit settingsSaved();
	changed = false;
}

void GlobalProxySettings::on_typeBox_2_currentIndexChanged(int type)
{
	if ( type )
	{
		ui.proxyHostEdit_2->setEnabled(true);
		ui.proxyPortBox_2->setEnabled(true);
		ui.authBox_2->setEnabled(true);
		if ( ui.authBox_2->isChecked() )
		{
			ui.userNameEdit_2->setEnabled(true);
			ui.userPasswordEdit_2->setEnabled(true);
		}
	} else {
		ui.userNameEdit_2->setEnabled(false);
		ui.userPasswordEdit_2->setEnabled(false);
		ui.proxyHostEdit_2->setEnabled(false);
		ui.proxyPortBox_2->setEnabled(false);
		ui.authBox_2->setEnabled(false);
	}
}

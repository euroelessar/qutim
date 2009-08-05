/*
    mainSettings

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

#include "mainsettings.h"

mainSettings::mainSettings(const QString &profile_name,
		QWidget *parent)
    : QWidget(parent)
    , m_iconManager(IconManager::instance())
    , m_profile_name(profile_name)
{
	ui.setupUi(this);
	changed = false;
	loadSettings();
	updateAccountComboBox();
	connect(ui.sizePosBox, SIGNAL(stateChanged(int)),
			this, SLOT(widgetStateChanged()));
	connect(ui.hideBox, SIGNAL(stateChanged(int)),
				this, SLOT(widgetStateChanged()));
	connect(ui.loginDialogBox, SIGNAL(stateChanged(int)),
					this, SLOT(widgetStateChanged()));
	connect(ui.menuAccountBox,SIGNAL(stateChanged(int)),
			this, SLOT(widgetStateChanged()));
	connect(ui.singleBox, SIGNAL(stateChanged(int)),
								this, SLOT(widgetStateChanged()));
	connect(ui.onTopBox, SIGNAL(stateChanged(int)),
								this, SLOT(widgetStateChanged()));
	connect(ui.autoAwayBox, SIGNAL(stateChanged(int)),
								this, SLOT(widgetStateChanged()));
	connect(ui.ahideBox, SIGNAL(stateChanged(int)),
								this, SLOT(widgetStateChanged()));
	connect(ui.secondsBox, SIGNAL(valueChanged(int)),
								this, SLOT(widgetStateChanged()));
	connect(ui.awayMinBox, SIGNAL(valueChanged(int)),
								this, SLOT(widgetStateChanged()));
	connect(ui.accountComboBox, SIGNAL(currentIndexChanged(int)),
								this, SLOT(widgetStateChanged()));
}

mainSettings::~mainSettings()
{
	
}


void mainSettings::loadSettings()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "qutim", "qutimsettings");
	QSettings profile_settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
	profile_settings.beginGroup("general");
	ui.sizePosBox->setChecked(profile_settings.value("savesizpos", true).toBool());
	ui.hideBox->setChecked(profile_settings.value("hidestart", false).toBool());
	ui.menuAccountBox->setChecked(profile_settings.value("accountmenu", true).toBool());
	ui.onTopBox->setChecked(profile_settings.value("ontop", false).toBool());
	ui.ahideBox->setChecked(profile_settings.value("autohide", false).toBool());
	ui.secondsBox->setValue(profile_settings.value("hidesecs", 60).toUInt());
	ui.autoAwayBox->setChecked(profile_settings.value("autoaway", true).toBool());
	ui.awayMinBox->setValue(profile_settings.value("awaymin", 10).toUInt());
	m_current_account_icon = profile_settings.value("currentaccount", "").toString();
	profile_settings.endGroup();
	ui.loginDialogBox->setChecked(settings.value("general/showstart", true).toBool());
	ui.singleBox->setChecked(settings.value("general/single", true).toBool());
}

void mainSettings::saveSettings()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "qutim", "qutimsettings");
	QSettings profile_settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
	profile_settings.beginGroup("general");
	profile_settings.setValue("savesizpos", ui.sizePosBox->isChecked());
	profile_settings.setValue("hidestart", ui.hideBox->isChecked());
	profile_settings.setValue("accountmenu", ui.menuAccountBox->isChecked());
	profile_settings.setValue("ontop", ui.onTopBox->isChecked());
	profile_settings.setValue("autohide", ui.ahideBox->isChecked());
	profile_settings.setValue("hidesecs", ui.secondsBox->value());
	profile_settings.setValue("autoaway", ui.autoAwayBox->isChecked());
	profile_settings.setValue("awaymin", ui.awayMinBox->value());
	m_current_account_icon = ui.accountComboBox->itemData(ui.accountComboBox->currentIndex()).toString();
	profile_settings.setValue("currentaccount", m_current_account_icon);
	profile_settings.endGroup();
	settings.setValue("general/showstart", ui.loginDialogBox->isChecked());
	settings.setValue("general/single", ui.singleBox->isChecked());
	if ( changed )
			emit settingsSaved();
	changed = false;
}

void mainSettings::updateAccountComboBox()
{
	ui.accountComboBox->clear();
	PluginSystem &ps = PluginSystem::instance();
	QList<AccountStructure> accounts_list = ps.getAccountsList();
	foreach ( AccountStructure account, accounts_list )
	{
		ui.accountComboBox->addItem(Icon(account.protocol_name.toLower(), IconInfo::Protocol),
									account.account_name,
									account.protocol_name + "." + account.account_name);
	}
	int account_index;
	if ( (account_index = ui.accountComboBox->findData(m_current_account_icon)) >= 0)
	{
		ui.accountComboBox->setCurrentIndex(account_index);
	}
	else
	{
		saveSettings();
	}
}

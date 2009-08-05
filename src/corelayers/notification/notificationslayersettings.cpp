/*
    NotificationsLayerSettings

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

#include "notificationslayersettings.h"
#include <QSettings>

NotificationsLayerSettings::NotificationsLayerSettings(const QString &profile_name,
		QWidget *parent)
    : QWidget(parent),
    m_profile_name(profile_name)
{
	ui.setupUi(this);
	changed = false;
	loadSettings();
	connect(ui.popupBox, SIGNAL(toggled(bool)),
				this, SLOT(widgetStateChanged()));
	connect(ui.widthSpinBox, SIGNAL(valueChanged(int)),
				this, SLOT(widgetStateChanged()));
	connect(ui.heightSpinBox, SIGNAL(valueChanged(int)),
				this, SLOT(widgetStateChanged()));
	connect(ui.timeSpinBox, SIGNAL(valueChanged(int)),
				this, SLOT(widgetStateChanged()));
	connect(ui.positionComboBox, SIGNAL(currentIndexChanged(int)),
				this, SLOT(widgetStateChanged()));
	connect(ui.styleComboBox, SIGNAL(currentIndexChanged(int)),
				this, SLOT(widgetStateChanged()));
	connect(ui.trayBalloonBox, SIGNAL(stateChanged(int)),
				this, SLOT(widgetStateChanged()));
	connect(ui.balloonSecBox, SIGNAL(valueChanged(int)),
				this, SLOT(widgetStateChanged()));
	connect(ui.signOnCheckBox, SIGNAL(stateChanged(int)),
				this, SLOT(widgetStateChanged()));
	connect(ui.signOffCheckBox, SIGNAL(stateChanged(int)),
				this, SLOT(widgetStateChanged()));
	connect(ui.typingCheckBox, SIGNAL(stateChanged(int)),
				this, SLOT(widgetStateChanged()));
	connect(ui.changeStatusCheckBox, SIGNAL(stateChanged(int)),
				this, SLOT(widgetStateChanged()));
	connect(ui.messageBox, SIGNAL(stateChanged(int)),
				this, SLOT(widgetStateChanged()));
}

NotificationsLayerSettings::~NotificationsLayerSettings()
{

}

void NotificationsLayerSettings::loadSettings()
{
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
	settings.beginGroup("notifications");
	ui.popupBox->setChecked(settings.value("popup", true).toBool());
        ui.widthSpinBox->setValue(settings.value("pwidth", 280).toInt());
        ui.heightSpinBox->setValue(settings.value("pheight", 110).toInt());
	ui.timeSpinBox->setValue(settings.value("ptime", 6).toInt());
	ui.positionComboBox->setCurrentIndex(settings.value("pposition", 3).toInt());
	ui.styleComboBox->setCurrentIndex(settings.value("pstyle", 1).toInt());
	ui.trayBalloonBox->setChecked(settings.value("balloon", false).toBool());
	ui.balloonSecBox->setValue(settings.value("bsecs", 5).toInt());
	ui.signOnCheckBox->setChecked(settings.value("signon", true).toBool());
	ui.signOffCheckBox->setChecked(settings.value("signoff", true).toBool());
	ui.typingCheckBox->setChecked(settings.value("typing", true).toBool());
	ui.changeStatusCheckBox->setChecked(settings.value("statuschange", true).toBool());
	ui.messageBox->setChecked(settings.value("message", true).toBool());
	settings.endGroup();
}

void NotificationsLayerSettings::saveSettings()
{
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
	settings.beginGroup("notifications");
	settings.setValue("popup", ui.popupBox->isChecked());
	settings.setValue("pwidth", ui.widthSpinBox->value());
	settings.setValue("pheight", ui.heightSpinBox->value());
	settings.setValue("ptime", ui.timeSpinBox->value());
	settings.setValue("pposition", ui.positionComboBox->currentIndex());
	settings.setValue("pstyle", ui.styleComboBox->currentIndex());
	settings.setValue("balloon", ui.trayBalloonBox->isChecked());
	settings.setValue("bsecs", ui.balloonSecBox->value());
	settings.setValue("signon", ui.signOnCheckBox->isChecked());
	settings.setValue("signoff", ui.signOffCheckBox->isChecked());
	settings.setValue("typing", ui.typingCheckBox->isChecked());
	settings.setValue("statuschange", ui.changeStatusCheckBox->isChecked());
	settings.setValue("message", ui.messageBox->isChecked());
	settings.endGroup();
	if ( changed )
			emit settingsSaved();
	changed = false;
}

/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
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

#include "birthdayremindersettings.h"
#include "ui_birthdayremindersettings.h"
#include <qutim/config.h>

BirthdayReminderSettings::BirthdayReminderSettings(QWidget *parent) :
	SettingsWidget(parent),
    ui(new Ui::BirthdayReminderSettings)
{
    ui->setupUi(this);
	listenChildrenStates();
}

BirthdayReminderSettings::~BirthdayReminderSettings()
{
    delete ui;
}

void BirthdayReminderSettings::loadImpl()
{
	Config cfg;
	cfg.beginGroup("birthdayReminder");
	ui->daysBeforeNotificationBox->setValue(cfg.value("daysBeforeNotification", 3));
	ui->intervalBetweenNotificationsBox->setValue(cfg.value("intervalBetweenNotifications", 24.0));
	cfg.endGroup();
}

void BirthdayReminderSettings::saveImpl()
{
	Config cfg;
	cfg.beginGroup("birthdayReminder");
	cfg.setValue("daysBeforeNotification", ui->daysBeforeNotificationBox->value());
	cfg.setValue("intervalBetweenNotifications", ui->intervalBetweenNotificationsBox->value());
	cfg.endGroup();
}

void BirthdayReminderSettings::cancelImpl()
{
	loadImpl();
}



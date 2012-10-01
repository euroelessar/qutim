/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#include "maemo5settings.h"
#include "ui_maemo5settings.h"
#include <qutim/config.h>


Maemo5Settings::Maemo5Settings() :
	ui(new Ui::Maemo5Settings)
{
	ui->setupUi(this);
}

Maemo5Settings::~Maemo5Settings()
{
	delete ui;
}

void Maemo5Settings::loadImpl()
{
	ui->ledPattern->addItem(QLatin1String("PatternError"));
	ui->ledPattern->addItem(QLatin1String("PatternDeviceOn"));
	ui->ledPattern->addItem(QLatin1String("PatternDeviceSoftOff"));
	ui->ledPattern->addItem(QLatin1String("PatternPowerOn"));
	ui->ledPattern->addItem(QLatin1String("PatternPowerOff"));
	ui->ledPattern->addItem(QLatin1String("PatternCommunicationCall"));
	ui->ledPattern->addItem(QLatin1String("PatternCommunicationIM"));
	ui->ledPattern->addItem(QLatin1String("PatternCommunicationSMS"));
	ui->ledPattern->addItem(QLatin1String("PatternCommunicationEmail"));
	ui->ledPattern->addItem(QLatin1String("PatternCommonNotification"));
	ui->ledPattern->addItem(QLatin1String("PatternWebcamActive"));
	ui->ledPattern->addItem(QLatin1String("PatternBatteryCharging"));
	ui->ledPattern->addItem(QLatin1String("PatternBatteryFull"));

	Config config = Config().group(QLatin1String("Maemo5"));

	int orientation = config.value(QLatin1String("orientation"),0);
	switch (orientation)
	{
	case 0:
		ui->autoOrientation->setChecked(true);
		break;
	case 1:
		ui->portrait->setChecked(true);
		break;
	case 2:
		ui->landscape->setChecked(true);
		break;
	}

	ui->displayOnLed->setChecked(config.value(QLatin1String("showLedWhenDisplayOn"),false));
	ui->displayOnVibration->setChecked(config.value(QLatin1String("vibrationWhenDisplayOn"),false));

	currentPattern = config.value(QLatin1String("ledPattern"),QLatin1String("PatternCommunicationIM"));
	int index = ui->ledPattern->findText(QLatin1String("PatternCommunicationIM"));
	ui->ledPattern->setCurrentIndex(index);

	ui->vibtarionTime->setValue(config.value(QLatin1String("vibrationTime"),50));
	ui->popupShowTime->setValue(config.value(QLatin1String("showPopupTime"),50));


	connect(ui->ledPattern,SIGNAL(currentIndexChanged(int)),SLOT(onSettingsChanged(int)));
	connect(ui->displayOnLed,SIGNAL(stateChanged(int)),SLOT(onSettingsChanged(int)));
	connect(ui->displayOnVibration,SIGNAL(stateChanged(int)),SLOT(onSettingsChanged(int)));
	connect(ui->vibtarionTime,SIGNAL(valueChanged(int)),SLOT(onSettingsChanged(int)));
	connect(ui->popupShowTime,SIGNAL(valueChanged(int)),SLOT(onSettingsChanged(int)));
	connect(ui->autoOrientation,SIGNAL(clicked()),SLOT(onSettingsChanged()));
	connect(ui->portrait,SIGNAL(clicked()),SLOT(onSettingsChanged()));
	connect(ui->landscape,SIGNAL(clicked()),SLOT(onSettingsChanged()));

}

void Maemo5Settings::saveImpl()
{
	Config config = Config().group(QLatin1String("Maemo5"));
	config.setValue(QLatin1String("showPopupTime"),ui->popupShowTime->value());
	config.setValue(QLatin1String("vibrationTime"),ui->vibtarionTime->value());
	config.setValue(QLatin1String("vibrationWhenDisplayOn"),ui->displayOnVibration->checkState());
	config.setValue(QLatin1String("showLedWhenDisplayOn"),ui->displayOnLed->checkState());
	config.setValue(QLatin1String("ledPattern"),ui->ledPattern->itemText(ui->ledPattern->currentIndex()));

	if (ui->autoOrientation->isChecked())
		config.setValue(QLatin1String("orientation"),0);
	else if (ui->portrait->isChecked())
		config.setValue(QLatin1String("orientation"),1);
	else if (ui->landscape->isChecked())
		config.setValue(QLatin1String("orientation"),2);

}

void Maemo5Settings::cancelImpl()
{

}

void Maemo5Settings::onSettingsChanged(int)
{
	setModified(true);
}
void Maemo5Settings::onSettingsChanged()
{
	setModified(true);
}


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


	connect(ui->ledPattern,SIGNAL(currentIndexChanged(int)),SLOT(onCheckedStateChanged(int)));
	connect(ui->displayOnLed,SIGNAL(stateChanged(int)),SLOT(onCheckedStateChanged(int)));
	connect(ui->displayOnVibration,SIGNAL(stateChanged(int)),SLOT(onCheckedStateChanged(int)));
	connect(ui->vibtarionTime,SIGNAL(valueChanged(int)),SLOT(onCheckedStateChanged(int)));
	connect(ui->popupShowTime,SIGNAL(valueChanged(int)),SLOT(onCheckedStateChanged(int)));
	connect(ui->autoOrientation,SIGNAL(clicked()),SLOT(onCheckedStateChanged(int)));
	connect(ui->portrait,SIGNAL(clicked()),SLOT(onCheckedStateChanged(int)));
	connect(ui->landscape,SIGNAL(clicked()),SLOT(onCheckedStateChanged(int)));

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

void Maemo5Settings::onCheckedStateChanged(int)
{
	emit modifiedChanged(true);
}



#include "pastersettings.h"
#include "handler.h"
#include <qutim/config.h>

using namespace qutim_sdk_0_3;

AutoPasterSettings::AutoPasterSettings()
	: ui(new Ui::Pastersettings)
{
	ui->setupUi(this);
	listenChildrenStates();
}

void AutoPasterSettings::loadImpl()
{
	ui->comboBox->clear();
	foreach (PasterInterface *paster, AutoPasterHandler::pasters())
		ui->comboBox->addItem(paster->name());

	Config cfg;
	cfg.beginGroup("autoPaster");
	ui->checkBox->setChecked(cfg.value(QLatin1String("autoSubmit"), false));
	ui->comboBox->setCurrentIndex(cfg.value(QLatin1String("defaultLocation"), 0));
	ui->spinBox->setValue(cfg.value(QLatin1String("lineCount"), 5));
}

void AutoPasterSettings::saveImpl()
{
	Config cfg;
	cfg.beginGroup("autoPaster");
	cfg.setValue(QLatin1String("autoSubmit"), ui->checkBox->isChecked());
	cfg.setValue(QLatin1String("defaultLocation"), ui->comboBox->currentIndex());
	cfg.setValue(QLatin1String("lineCount"), ui->spinBox->value());
}

void AutoPasterSettings::cancelImpl()
{
	loadImpl();
}

AutoPasterSettings::~AutoPasterSettings()
{
	delete ui;
}

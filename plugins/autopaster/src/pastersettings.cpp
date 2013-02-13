#include "pastersettings.h"
#include "handler.h"
#include <qutim/config.h>

using namespace qutim_sdk_0_3;

AutoPasterSettings::AutoPasterSettings()
	: ui(new Ui::Pastersettings)
{
	ui->setupUi(this);
	foreach (PasterInterface *paster, AutoPasterHandler::pasters())
		ui->comboBox->addItem(paster->name());
	lookForWidgetState(ui->checkBox);
	lookForWidgetState(ui->comboBox);
	lookForWidgetState(ui->spinBox);
	lookForWidgetState(ui->spinBox_2);
	ui->spinBox_2->setDisabled(true);
}

void AutoPasterSettings::loadImpl()
{
	Config cfg;
	cfg.beginGroup("AutoPaster");
	ui->checkBox->setChecked(cfg.value(QLatin1String("AutoSubmit"), false));
	ui->comboBox->setCurrentIndex(cfg.value(QLatin1String("DefaultLocation"), 0));
	ui->spinBox->setValue(cfg.value(QLatin1String("LineCount"), 5));
	ui->spinBox_2->setValue(cfg.value(QLatin1String("Delay"), 15000));
	cfg.endGroup();
}

void AutoPasterSettings::saveImpl()
{
	Config cfg;
	cfg.beginGroup("AutoPaster");
	cfg.setValue(QLatin1String("AutoSubmit"), ui->checkBox->isChecked());
	cfg.setValue(QLatin1String("DefaultLocation"), ui->comboBox->currentIndex());
	cfg.setValue(QLatin1String("LineCount"), ui->spinBox->value());
	cfg.setValue(QLatin1String("Delay"), ui->spinBox_2->value());
	cfg.endGroup();
}

void AutoPasterSettings::cancelImpl()
{
	loadImpl();
}

AutoPasterSettings::~AutoPasterSettings()
{
	delete ui;
}

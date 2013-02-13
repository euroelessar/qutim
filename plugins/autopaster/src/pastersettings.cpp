#include "pastersettings.h"
#include <qutim/config.h>

using namespace qutim_sdk_0_3;

PasterSettings::PasterSettings():qutim_sdk_0_3::SettingsWidget(),
	ui(new Ui::Pastersettings)
{
	ui->setupUi(this);
	ui->comboBox->addItem("paste.ubuntu.com","http://paste.ubuntu.com");
	ui->comboBox->addItem("hastebin.com","http://hastebin.com/");
	lookForWidgetState(ui->checkBox);
	lookForWidgetState(ui->comboBox);
	lookForWidgetState(ui->spinBox);
	lookForWidgetState(ui->spinBox_2);
	ui->spinBox_2->setDisabled(true);
	QObject::connect(ui->checkBox,SIGNAL(toggled(bool)),this,SLOT(checkAutoSubmit()));
}

void PasterSettings::loadImpl()
{
	Config cfg;
	cfg.beginGroup("AutoPaster");
	ui->checkBox->setChecked(cfg.value(QLatin1String("AutoSubmit"), true));
	ui->comboBox->setCurrentIndex(cfg.value(QLatin1String("DefaultLocation"),0));
	ui->spinBox->setValue(cfg.value(QLatin1String("LineCount"),5));
	ui->spinBox_2->setValue(cfg.value(QLatin1String("Delay"),1000));
	cfg.endGroup();
}

void PasterSettings::saveImpl()
{
	Config cfg;
	cfg.beginGroup("AutoPaster");
	cfg.setValue(QLatin1String("AutoSubmit"),ui->checkBox->isChecked());
	cfg.setValue(QLatin1String("DefaultLocation"),QString::number(ui->comboBox->currentIndex()));
	cfg.setValue(QLatin1String("LineCount"),ui->spinBox->value());
	cfg.setValue(QLatin1String("Delay"),ui->spinBox_2->value());
	cfg.endGroup();
}

void PasterSettings::cancelImpl()
{
	loadImpl();
}

PasterSettings::~PasterSettings()
{
	delete ui;
}

void PasterSettings::checkAutoSubmit()
{
	ui->spinBox_2->setEnabled(ui->checkBox->isChecked());
}

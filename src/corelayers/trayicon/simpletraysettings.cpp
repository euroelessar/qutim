#include "simpletraysettings.h"
#include "ui_simpletraysettings.h"
#include <qutim/config.h>

namespace Core {
using namespace qutim_sdk_0_3;

const char * traySettingsFilename = "simpletray";

SimpletraySettings::SimpletraySettings(QWidget *parent) :
	SettingsWidget(),
	ui(new Ui::SimpletraySettings)
{
	ui->setupUi(this);
	lookForWidgetState(ui->cb_blink);
	lookForWidgetState(ui->cb_showIcon);
	lookForWidgetState(ui->rb_showMsgsNumber);
	lookForWidgetState(ui->rb_showSessNumber);
	lookForWidgetState(ui->rb_dontShowNumber);
}

SimpletraySettings::~SimpletraySettings()
{
	delete ui;
}

void SimpletraySettings::loadImpl()
{
	Config cfg(traySettingsFilename);
	ui->cb_blink->   setChecked(cfg.value("blink",    true));
	ui->cb_showIcon->setChecked(cfg.value("showIcon", true));
	ui->cb_showIcon->click();
	ui->cb_showIcon->click(); // HACK: this CB should emit clicked(bool)
	const int option = cfg.value("showNumber", DontShow);
	switch (option) {
	case ShowMsgsNumber : ui->rb_showMsgsNumber->setChecked(true); break;
	case ShowSessNumber : ui->rb_showSessNumber->setChecked(true); break;
	default : ui->rb_dontShowNumber->setChecked(true); break;
	}
}

void SimpletraySettings::saveImpl()
{
	Config cfg(traySettingsFilename);
	cfg.setValue("blink",    ui->cb_blink->   isChecked());
	cfg.setValue("showIcon", ui->cb_showIcon->isChecked());
	int option;
	if (ui->rb_showMsgsNumber->isChecked())
		option = ShowMsgsNumber;
	else if (ui->rb_showSessNumber->isChecked())
		option = ShowSessNumber;
	else
		option = DontShow;
	cfg.setValue("showNumber", option);
}

void SimpletraySettings::cancelImpl()
{
	loadImpl();
}

}


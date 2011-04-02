#include "testsettings.h"
#include <qutim/config.h>

using namespace qutim_sdk_0_3;

TestSettings::TestSettings()
	: ui(new Ui::TestSettingsForm)
{
	ui->setupUi(this);
	lookForWidgetState(ui->checkbox);
}

void TestSettings::loadImpl()
{
	Config cfg("testplugin");
	ui->checkbox->setChecked(cfg.value("showmsgbox", false));
}

void TestSettings::saveImpl()
{
	Config cfg("testplugin");
	cfg.setValue("showmsgbox", ui->checkbox->isChecked());
}

void TestSettings::cancelImpl()
{
	loadImpl();
}

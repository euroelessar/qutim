/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
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

#include "simpletraysettings.h"
#include "ui_simpletraysettings.h"
#include <qutim/config.h>

namespace Core {
using namespace qutim_sdk_0_3;

const char * traySettingsFilename = "simpletray";

SimpletraySettings::SimpletraySettings(QWidget *parent) :
	SettingsWidget(parent),
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
	const int option = cfg.value("showNumber", CounterDontShow);
	switch (option) {
	case CounterShowMessages:
		ui->rb_showMsgsNumber->setChecked(true);
		break;
	case CounterShowSessions:
		ui->rb_showSessNumber->setChecked(true);
		break;
	default:
		ui->rb_dontShowNumber->setChecked(true);
		break;
	}
}

void SimpletraySettings::saveImpl()
{
	Config cfg(traySettingsFilename);
	cfg.setValue("blink",    ui->cb_blink->   isChecked());
	cfg.setValue("showIcon", ui->cb_showIcon->isChecked());
	int option;
	if (ui->rb_showMsgsNumber->isChecked())
		option = CounterShowMessages;
	else if (ui->rb_showSessNumber->isChecked())
		option = CounterShowSessions;
	else
		option = CounterDontShow;
	cfg.setValue("showNumber", option);
}

void SimpletraySettings::cancelImpl()
{
	loadImpl();
}

}



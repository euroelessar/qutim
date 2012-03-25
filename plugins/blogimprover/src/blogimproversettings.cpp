/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Nicolay Izoderov <nico-izo@yandex.ru>
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

#include "blogimproversettings.h"
#include <qutim/config.h>

using namespace qutim_sdk_0_3;

BlogImproverSettings::BlogImproverSettings()
	: ui(new Ui::BlogImproverSettingsForm)
{
	ui->setupUi(this);
	lookForWidgetState(ui->enablePstoIntegration);
	lookForWidgetState(ui->enableJuickIntegration);
	lookForWidgetState(ui->enableBnwIntegration);
}

void BlogImproverSettings::loadImpl()
{
	Config cfg;
	cfg.beginGroup("BlogImprover");
	ui->enablePstoIntegration->setChecked(cfg.value("enablePstoIntegration", true));
	ui->enableJuickIntegration->setChecked(cfg.value("enableJuickIntegration", true));
	ui->enableBnwIntegration->setChecked(cfg.value("enableBnwIntegration", true));
	cfg.endGroup();
}

void BlogImproverSettings::saveImpl()
{
	Config cfg;
	cfg.beginGroup("BlogImprover");
	cfg.setValue("enablePstoIntegration", ui->enablePstoIntegration->isChecked());
	cfg.setValue("enableJuickIntegration", ui->enableJuickIntegration->isChecked());
	cfg.setValue("enableBnwIntegration", ui->enableBnwIntegration->isChecked());
	cfg.endGroup();
}

void BlogImproverSettings::cancelImpl()
{
	loadImpl();
}

BlogImproverSettings::~BlogImproverSettings()
{
	delete ui;
}

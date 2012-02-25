/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Nicolay Izoderov <nico-izo@yandex.ru>
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

#include "highlightersettings.h"
#include <qutim/config.h>

using namespace qutim_sdk_0_3;

HighlighterSettings::HighlighterSettings()
	: ui(new Ui::HighlighterSettingsForm)
{
	ui->setupUi(this);
	lookForWidgetState(ui->enableHighlights);
	lookForWidgetState(ui->regexp);
}

void HighlighterSettings::loadImpl()
{
	Config cfg;
	cfg.beginGroup("highlighter");
	ui->regexp->setText(cfg.value("regexp", ""));
	ui->enableHighlights->setChecked(cfg.value("enableHighlights", true));
	cfg.endGroup();
}

void HighlighterSettings::saveImpl()
{
	Config cfg;
	cfg.beginGroup("highlighter");
	cfg.setValue("regexp", ui->regexp->text());
	cfg.setValue("enableHighlights", ui->enableHighlights->isChecked());
	cfg.endGroup();
}

void HighlighterSettings::cancelImpl()
{
	loadImpl();
}

HighlighterSettings::~HighlighterSettings()
{
	delete ui;
}


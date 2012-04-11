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
#include "sessionhelpersettings.h"
#include "ui_sessionhelpersettings.h"
#include <qutim/config.h>
#include <qutim/localizedstring.h>
#include <qutim/status.h>

namespace Core
{

SessionHelperSettings::SessionHelperSettings() :
		ui(new Ui::SessionHelperSettings)
{
	ui->setupUi(this);
	lookForWidgetState(ui->activateMultichatBox);
}

SessionHelperSettings::~SessionHelperSettings()
{
	delete ui;
}

void SessionHelperSettings::cancelImpl()
{
	loadImpl();
}

void SessionHelperSettings::loadImpl()
{
	Config config("appearance");
	config.beginGroup("chat/behavior/widget");
	ui->activateMultichatBox->setChecked(config.value("activateMultichat", true));
}

void SessionHelperSettings::saveImpl()
{
	Config config("appearance");
	config.beginGroup("chat/behavior/widget");
	config.setValue("activateMultichat", ui->activateMultichatBox->isChecked());
}

}


/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin euroelessar@yandex.ru
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
#include "autosettings.h"
#include  "modulemanagerimpl.h" 
#include <libqutim/settingslayer.h>
#include <QCheckBox>

namespace Core
{
static CoreModuleHelper<AutoSettings, StartupModule> autosettings_settings_static(
	QT_TRANSLATE_NOOP("Plugin", "Autosettings example"),
	QT_TRANSLATE_NOOP("Plugin", "Simple example of auto settings item generator")
	);

AutoSettings::AutoSettings()
{
	AutoSettingsItem *item = new AutoSettingsItem(Settings::General, QT_TRANSLATE_NOOP("Settings", "Test settings"));
	item->setConfig(QString(), "test");
	item->addEntry<QLineEdit>(QT_TRANSLATE_NOOP("Settings", "Text"))->setName("text")->setProperty("acceptRichText", false);
	item->addEntry<AutoSettingsComboBox>(QT_TRANSLATE_NOOP("Settings", "Combo"))->setName("combo")->setProperty("items", QStringList() << "First" << "Second" << "Third");
	item->addEntry<QCheckBox>(QT_TRANSLATE_NOOP("Settings", "Check"))->setName("check");
	Settings::registerItem(item);
}
}


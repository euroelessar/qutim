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

#include "localizationplugin.h"
#include "localizationmodule.h"
#include <qutim/plugin.h>
#include <qutim/systeminfo.h>
#include <QDebug>

using namespace qutim_sdk_0_3;

namespace Core
{
void LocalizationPlugin::init()
{
	ExtensionIcon icon;
	LocalizedString name = QT_TRANSLATE_NOOP("Plugin", "Localization");
	LocalizedString description = QT_TRANSLATE_NOOP("Plugin", "Default qutIM localization engine");
	setInfo(name, description, QUTIM_VERSION, icon);
	addExtension<Core::LocalizationModule,qutim_sdk_0_3::StartupModule>(name, description, icon);
	
	QStringList langs = LocalizationModule::determineSystemLocale();
	if (!langs.isEmpty())
		LocalizationModule::loadLanguage(langs);
}

bool LocalizationPlugin::load()
{
	return true;
}

bool LocalizationPlugin::unload()
{
	return true;
}
}

QUTIM_EXPORT_PLUGIN(Core::LocalizationPlugin)


/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2011 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#include "localizationplugin.h"
#include "localizationmodule.h"
#include <qutim/plugin.h>

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


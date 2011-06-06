/****************************************************************************
 * ulplugin.cpp
 *  Copyright © 2010-2011, Vsevolod Velichko <torkvema@gmail.com>.
 *  Licence: GPLv2 or later
 *
 ****************************************************************************
 *                                                                          *
 *   This library is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation; either version 2 of the License, or      *
 *   (at your option) any later version.                                    *
 *                                                                          *
 ****************************************************************************/

#include "ulplugin.h"
#include <qutim/debug.h>
#include <qutim/icon.h>

UnityLauncherPlugin::UnityLauncherPlugin ()
{
}

void UnityLauncherPlugin::init ()
{
	qutim_sdk_0_3::ExtensionIcon icon("info");
	addAuthor(QT_TRANSLATE_NOOP("Author", "Vsevolod Velichko"),
			QT_TRANSLATE_NOOP("Task", "Developer"),
			QLatin1String("torkvema@gmail.com")
			);
	setInfo(QT_TRANSLATE_NOOP("Plugin", "UnityLauncher"),
			QT_TRANSLATE_NOOP("Plugin", "Ubuntu Unity panel integration"),
			PLUGIN_VERSION(0, 0, 1, 1),
			icon
			);
	setCapabilities(Loadable);
}

bool UnityLauncherPlugin::load ()
{
	qutim_sdk_0_3::debug() << "[UnityLauncher] Plugin load";
	service = new UnityLauncherService();

	return true;
}

bool UnityLauncherPlugin::unload ()
{
	qutim_sdk_0_3::debug() << "[UnityLauncher] Plugin unload";
	delete service.data();
	return true;
}

QUTIM_EXPORT_PLUGIN(UnityLauncherPlugin)

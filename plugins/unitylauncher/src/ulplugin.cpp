/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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


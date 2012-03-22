/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Nicolay Izoderov <nico-izo@ya.ru>
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
#include "haikunotificationsplugin.h"
#include "haikunotificationsbackend.h"

HaikuNotificationsPlugin::HaikuNotificationsPlugin()
{
}

void HaikuNotificationsPlugin::init()
{
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Haiku notifications"),
		QT_TRANSLATE_NOOP("Plugin", "Notification system integrated into Haiku using native BeAPI"),
		PLUGIN_VERSION(0, 0, 1, 0));
	addAuthor(QLatin1String("nicoizo"));
	addExtension<HaikuNotificationsBackend>(QT_TRANSLATE_NOOP("plugin","Haiku notifications"),
							   QT_TRANSLATE_NOOP("plugin","Notification system integrated into Haiku using native BeAPI"));
}

bool HaikuNotificationsPlugin::load()
{
	return true;
}

bool HaikuNotificationsPlugin::unload()
{
	return false;
}


QUTIM_EXPORT_PLUGIN(HaikuNotificationsPlugin)


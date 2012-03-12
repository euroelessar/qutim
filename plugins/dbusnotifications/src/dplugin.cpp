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
#include "dplugin.h"
#include "dbusbackend.h"
#include <qutim/debug.h>


DPlugin::DPlugin()
{
	debug() << Q_FUNC_INFO;
}

void DPlugin::init()
{
	debug() << Q_FUNC_INFO;
	ExtensionIcon dIcon("mac");
	setInfo(QT_TRANSLATE_NOOP("Plugin", "DBus notifications"),
		QT_TRANSLATE_NOOP("Plugin", "Notification system based on Freedesktop DBus protocol"),
		PLUGIN_VERSION(0, 0, 1, 0),
		dIcon);
	addAuthor(QLatin1String("sauron"));
	addExtension<DBusBackend>(QT_TRANSLATE_NOOP("plugin","DBus notifications"),
							   QT_TRANSLATE_NOOP("plugin","Notification system based on Freedesktop DBus protocol"));
}

bool DPlugin::load()
{
	return true;
}

bool DPlugin::unload()
{
	return false;
}


QUTIM_EXPORT_PLUGIN(DPlugin)


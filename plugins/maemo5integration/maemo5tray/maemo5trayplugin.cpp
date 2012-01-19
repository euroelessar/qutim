/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Evgeniy Degtyarev <degtep@gmail.com>
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

#include "maemo5trayplugin.h"
#include "maemo5tray.h"

Maemo5TrayPlugin::Maemo5TrayPlugin()
{
}

void Maemo5TrayPlugin::init()
{
	addAuthor(QT_TRANSLATE_NOOP("Author", "Evgeniy Degtyarev"),
			  QT_TRANSLATE_NOOP("Task", "Author"),
			  QLatin1String("degtep@gmail.com"));
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Maemo 5 Tray"),
			QT_TRANSLATE_NOOP("Plugin", "Implement Maemo 5 specific system tray"),
			PLUGIN_VERSION(0, 0, 1, 0));

	addExtension<Maemo5Tray>(QT_TRANSLATE_NOOP("Plugin", "Maemo 5 Tray"),
								QT_TRANSLATE_NOOP("Plugin", "Implement Maemo 5 specific system tray"));

}

bool Maemo5TrayPlugin::load()
{
	return true;
}

bool Maemo5TrayPlugin::unload()
{
	return false;
}

QUTIM_EXPORT_PLUGIN(Maemo5TrayPlugin)


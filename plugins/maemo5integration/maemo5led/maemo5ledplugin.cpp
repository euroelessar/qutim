/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Evgeniy Degtyarev <degtep@gmail.com>
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

#include "maemo5ledplugin.h"
#include "maemo5led.h"


Maemo5LedPlugin::Maemo5LedPlugin()
{
	qDebug("%s", Q_FUNC_INFO);
}

void Maemo5LedPlugin::init()
{
	qDebug("%s", Q_FUNC_INFO);
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Maemo Led Notifications"),
		QT_TRANSLATE_NOOP("Plugin", "Notification system based on Maemo 5 Led"),
		PLUGIN_VERSION(0, 0, 1, 0));
	addAuthor(QT_TRANSLATE_NOOP("Author","Evgeniy Degtyarev"),
			  QT_TRANSLATE_NOOP("Task","Author"),
			  QLatin1String("degtep@gmail.com"));
	addExtension<Maemo5Led>(QT_TRANSLATE_NOOP("plugin","Maemo Led Notifications"),
							   QT_TRANSLATE_NOOP("plugin","Notification system, based on Maemo 5 Led"));
}

bool Maemo5LedPlugin::load()
{
	return true;
}

bool Maemo5LedPlugin::unload()
{
	return false;
}


QUTIM_EXPORT_PLUGIN(Maemo5LedPlugin)


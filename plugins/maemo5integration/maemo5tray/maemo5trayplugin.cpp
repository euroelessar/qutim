/****************************************************************************
 *  maemo5trayplugin.cpp
 *
 *  Copyright (c) 2011 by Evgeniy Degtyarev <degtep@gmail.comu>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

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

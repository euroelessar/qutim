/****************************************************************************
 *  oscarplugin.cpp
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#include "oscarplugin.h"
#include "icqprotocol.h"

OscarPlugin::OscarPlugin()
{
	qDebug("%s", Q_FUNC_INFO);
}

void OscarPlugin::init()
{
	qDebug("%s", Q_FUNC_INFO);
	m_info = PluginInfo(QT_TRANSLATE_NOOP("Plugin", "Oscar"),
						QT_TRANSLATE_NOOP("Plugin", "Module-based realization of Oscar protocol"),
						PLUGIN_VERSION(0, 0, 1, 0));
	m_info.addAuthor(QT_TRANSLATE_NOOP("Author", "Ruslan Nigmatullin"),
					 QT_TRANSLATE_NOOP("Task", "Author"),
					 QLatin1String("euroelessar@gmail.com"));
	m_extensions << ExtensionInfo(QT_TRANSLATE_NOOP("Plugin", "ICQ"),
								  QT_TRANSLATE_NOOP("Plugin", "Module-based realization of ICQ protocol"),
								  new GeneralGenerator<IcqProtocol>());
}

bool OscarPlugin::load()
{
	return true;
}

bool OscarPlugin::unload()
{
	return false;
}

QUTIM_EXPORT_PLUGIN(OscarPlugin)

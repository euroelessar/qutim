/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#include "sdlplugin.h"
#include "sdlbackend.h"

void SDLSoundPlugin::init()
{
	addAuthor(QT_TRANSLATE_NOOP("Author", "Ruslan Nigmatullin"),
			  QT_TRANSLATE_NOOP("Task", "Author"),
			  QLatin1String("euroelessar@gmail.com"));
	setInfo(QT_TRANSLATE_NOOP("Plugin", "SDL sound engine"),
			QT_TRANSLATE_NOOP("Plugin", "Sound engine based on Simple DirectMedia Layer"),
			PLUGIN_VERSION(0, 1, 0, 0),
			ExtensionIcon());
	addExtension<SDLSoundBackend>(
			QT_TRANSLATE_NOOP("Plugin", "SDL sound engine"),
			QT_TRANSLATE_NOOP("Plugin", "Sound engine based on Simple DirectMedia Layer"),
			ExtensionIcon());
}

bool SDLSoundPlugin::load()
{
	return true;
}

bool SDLSoundPlugin::unload()
{
	return false;
}

QUTIM_EXPORT_PLUGIN(SDLSoundPlugin)

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

#include "phononsoundplugin.h"
#include "phononsoundbackend.h"

void PhononSoundPlugin::init()
{
	addAuthor(QT_TRANSLATE_NOOP("Author", "Ruslan Nigmatullin"),
			  QT_TRANSLATE_NOOP("Task", "Author"),
			  QLatin1String("euroelessar@gmail.com"));
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Phonon sound engine"),
			QT_TRANSLATE_NOOP("Plugin", "Sound engine based on KDE sound engine"),
			PLUGIN_VERSION(0, 1, 0, 0),
			ExtensionIcon());
	addExtension<PhononSoundBackend>(
			QT_TRANSLATE_NOOP("Plugin", "Phonon sound engine"),
			QT_TRANSLATE_NOOP("Plugin", "Sound engine based on KDE sound engine"),
			ExtensionIcon());
}

bool PhononSoundPlugin::load()
{
	return true;
}

bool PhononSoundPlugin::unload()
{
	return false;
}

QUTIM_EXPORT_PLUGIN(PhononSoundPlugin)

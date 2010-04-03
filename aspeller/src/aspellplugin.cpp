/****************************************************************************
 * aspellplugin.cpp
 *
 *  Copyright (c) 2010 by Ruslan Nigmatullin <euroelessar@gmail.com>
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

#include "aspellplugin.h"
#include "aspellchecker.h"

ASpellPlugin::ASpellPlugin()
{
}

void ASpellPlugin::init()
{
	ExtensionIcon icon("tools-check-spelling");
	addAuthor(QT_TRANSLATE_NOOP("Author", "Ruslan Nigmatullin"),
			  QT_TRANSLATE_NOOP("Task", "Developer"),
			  QLatin1String("euroelessar@gmail.com"));
	setInfo(QT_TRANSLATE_NOOP("Plugin", "ASpell checker"),
			QT_TRANSLATE_NOOP("Plugin", "Check spelling by ASpell library"),
			PLUGIN_VERSION(0, 0, 1, 0), icon);
	addExtension<ASpellChecker>(QT_TRANSLATE_NOOP("Plugin", "ASpell checker"),
								QT_TRANSLATE_NOOP("Plugin", "Check spelling by ASpell library"),
								icon);
}

bool ASpellPlugin::load()
{
	return true;
}

bool ASpellPlugin::unload()
{
	return false;
}

QUTIM_EXPORT_PLUGIN(ASpellPlugin)

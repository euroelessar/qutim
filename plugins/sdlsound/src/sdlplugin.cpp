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

#include "sdlplugin.h"
#include "sdlbackend.h"

void SDLSoundPlugin::init()
{
	addAuthor(QLatin1String("euroelessar"));
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


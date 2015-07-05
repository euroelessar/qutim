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

#include "aspellplugin.h"
#include "aspellchecker.h"

ASpellPlugin::ASpellPlugin()
{
}

void ASpellPlugin::init()
{
	ExtensionIcon icon("tools-check-spelling");
	addAuthor(QLatin1String("euroelessar"));
	addAuthor(QT_TRANSLATE_NOOP("Author", "Alexey Prokhin"),
			  QT_TRANSLATE_NOOP("Task", "Author"),
			  QLatin1String("alexey.prokhin@yandex.ru"));
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


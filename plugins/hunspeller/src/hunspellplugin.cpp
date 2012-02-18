/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
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

#include "hunspellplugin.h"
#include "hunspellchecker.h"

HunSpellPlugin::HunSpellPlugin()
{
}

void HunSpellPlugin::init()
{
	ExtensionIcon icon("tools-check-spelling");
	addAuthor(QT_TRANSLATE_NOOP("Author", "Alexey Prokhin"),
			  QT_TRANSLATE_NOOP("Task", "Author"),
			  QLatin1String("alexey.prokhin@yandex.ru"));
	setInfo(QT_TRANSLATE_NOOP("Plugin", "HunSpell checker"),
			QT_TRANSLATE_NOOP("Plugin", "Check spelling by HunSpell library"),
			PLUGIN_VERSION(0, 0, 1, 0), icon);
	addExtension<HunSpellChecker>(QT_TRANSLATE_NOOP("Plugin", "HunSpell checker"),
								QT_TRANSLATE_NOOP("Plugin", "Check spelling by HunSpell library"),
								icon);
}

bool HunSpellPlugin::load()
{
	return true;
}

bool HunSpellPlugin::unload()
{
	return false;
}

QUTIM_EXPORT_PLUGIN(HunSpellPlugin)


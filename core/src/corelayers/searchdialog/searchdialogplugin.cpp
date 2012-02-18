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

#include "searchdialogplugin.h"
#include "searchdialoglayer.h"
#include "defaultform/defaultsearchform.h"
#include "mobileform/mobilesearchform.h"

namespace Core
{
	void SearchDialogPlugin::init()
	{
		setInfo(QT_TRANSLATE_NOOP("Plugin", "Search window"),
				QT_TRANSLATE_NOOP("Plugin", "Default qutIM implementation of search window"),
				QUTIM_VERSION);
		addAuthor(QT_TRANSLATE_NOOP("Author", "Alexey Prokhin"),
				  QT_TRANSLATE_NOOP("Task", "Author"),
				  QLatin1String("alexey.prokhin@yandex.ru"));
		addExtension<SearchLayer>(
				QT_TRANSLATE_NOOP("Plugin", "Search window"),
				QT_TRANSLATE_NOOP("Plugin", "Default qutIM implementation of search window"));
		addExtension<DefaultSearchFormFactory>(
				QT_TRANSLATE_NOOP("Plugin", "Desktop Search Form"),
				QT_TRANSLATE_NOOP("Plugin", "Default search form optimized for Desktop systems"));
		addExtension<MobileSearchFormFactory>(
				QT_TRANSLATE_NOOP("Plugin", "Mobile Search Form"),
				QT_TRANSLATE_NOOP("Plugin", "Search form optimized for Mobile systems"));
	}

	bool SearchDialogPlugin::load()
	{
		return true;
	}

	bool SearchDialogPlugin::unload()
	{
		return false;
	}
}

QUTIM_EXPORT_PLUGIN(Core::SearchDialogPlugin)


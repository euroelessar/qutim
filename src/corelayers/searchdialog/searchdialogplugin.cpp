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

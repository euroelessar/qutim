/****************************************************************************
 *  historymanagerplugin.cpp
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#include "historymanagerplugin.h"
#include "historymanagerwindow.h"
#include <qutim/menucontroller.h>
#include <qutim/icon.h>
#include <QDebug>
#include <QFile>

void HistoryManagerPlugin::init()
{
	addAuthor(QT_TRANSLATE_NOOP("Author", "Ruslan Nigmatullin"),
		QT_TRANSLATE_NOOP("Task", "Author"),
		QLatin1String("euroelessar@gmail.com"));
	setInfo(QT_TRANSLATE_NOOP("Plugin", "HistMan"),
			QT_TRANSLATE_NOOP("Plugin", "Plugin makes possible to import history to qutIM from different messengers"),
			PLUGIN_VERSION(0, 2, 0, 0),
			ExtensionIcon("view-history"));
}

bool HistoryManagerPlugin::load()
{
	if (MenuController *menu = qobject_cast<MenuController*>(getService("ContactList"))) {
		menu->addAction(new ActionGenerator(Icon("view-history"),
											QT_TRANSLATE_NOOP("Plugin", "Import history"),
											this,
											SLOT(createWidget())));
		return true;
	}
	return false;
}

bool HistoryManagerPlugin::unload()
{
	return true;
}

void HistoryManagerPlugin::createWidget()
{
	if(!m_widget) {
		m_widget = new HistoryManager::HistoryManagerWindow();
		m_widget->show();
	}
}

QUTIM_EXPORT_PLUGIN(HistoryManagerPlugin)

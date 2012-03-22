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

#include "historymanagerplugin.h"
#include "historymanagerwindow.h"
#include <qutim/menucontroller.h>
#include <qutim/icon.h>
#include <qutim/servicemanager.h>
#include <QDebug>
#include <QFile>

void HistoryManagerPlugin::init()
{
	addAuthor(QLatin1String("euroelessar"));
	setInfo(QT_TRANSLATE_NOOP("Plugin", "HistMan"),
			QT_TRANSLATE_NOOP("Plugin", "Plugin makes possible to import history to qutIM from different messengers"),
			PLUGIN_VERSION(0, 2, 0, 0),
			ExtensionIcon("view-history"));
	setCapabilities(Loadable);
}

bool HistoryManagerPlugin::load()
{
	if (MenuController *menu = qobject_cast<MenuController*>(ServiceManager::getByName("ContactList"))) {
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
		m_widget.data()->show();
	}
}

QUTIM_EXPORT_PLUGIN(HistoryManagerPlugin)


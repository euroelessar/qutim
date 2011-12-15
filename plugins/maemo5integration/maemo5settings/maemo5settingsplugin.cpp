/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Evgeniy Degtyarev <degtep@gmail.com>
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
#include "maemo5settingsplugin.h"
#include <qutim/icon.h>


Maemo5SettingsPlugin::Maemo5SettingsPlugin()
{
	qDebug("%s", Q_FUNC_INFO);
}

void Maemo5SettingsPlugin::init()
	{
		setInfo(QT_TRANSLATE_NOOP("Plugin", "Maemo 5 Settings"),
		QT_TRANSLATE_NOOP("Plugin", "Specific Maemo 5 settings"),
		PLUGIN_VERSION(0, 0, 1, 0));
	addAuthor(QT_TRANSLATE_NOOP("Author","Evgeniy Degtyarev"),
			  QT_TRANSLATE_NOOP("Task","Author"),
			  QLatin1String("degtep@gmail.com"));
	addExtension<Maemo5Settings>(QT_TRANSLATE_NOOP("plugin","Maemo 5 Settings"),
							   QT_TRANSLATE_NOOP("plugin","Specific Maemo 5 settings"));
m_item = new GeneralSettingsItem<Maemo5Settings>(Settings::Plugin, Icon("maemo5"), QT_TRANSLATE_NOOP("Settings","Maemo 5 Settings"));
	Settings::registerItem(m_item);
	
	}
bool Maemo5SettingsPlugin::load() {return true; }
bool Maemo5SettingsPlugin::unload() { return true; }
	
QUTIM_EXPORT_PLUGIN(Maemo5SettingsPlugin)


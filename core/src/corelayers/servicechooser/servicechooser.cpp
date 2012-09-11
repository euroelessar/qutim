/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#include "servicechooser.h"
#include "servicechooserwidget.h"
#include "protocolchooserwidget.h"
#include "pluginchooserwidget.h"
#include <qutim/settingslayer.h>
#include <qutim/icon.h>
#include <qutim/extensioninfo.h>

namespace Core
{
ServiceChoooser::ServiceChoooser(QObject* parent): QObject(parent)
{
	GeneralSettingsItem<ServiceChoooserWidget> *item = new GeneralSettingsItem<ServiceChoooserWidget>(Settings::Plugin, Icon("applications-system"), QT_TRANSLATE_NOOP("Settings","Service chooser"));
	Settings::registerItem(item);
	GeneralSettingsItem<ProtocolChoooserWidget> *item2 = new GeneralSettingsItem<ProtocolChoooserWidget>(Settings::Protocol, Icon("applications-system"), QT_TRANSLATE_NOOP("Settings","Protocol chooser"));
	Settings::registerItem(item2);
	GeneralSettingsItem<PluginChoooserWidget> *item3 = new GeneralSettingsItem<PluginChoooserWidget>(Settings::Plugin, Icon("applications-system"), QT_TRANSLATE_NOOP("Settings","Plugin chooser"));
	Settings::registerItem(item3);
	deleteLater();
}

const char *ServiceChoooser::className(const ExtensionInfo &info)
{
	return info.generator()->metaObject()->className();
}

QString ServiceChoooser::html(const qutim_sdk_0_3::ExtensionInfo& info)
{
	QString html = tr("<b>Name: </b> %1 <br />").arg(info.name());
	html += tr("<b>Description: </b> %1 <br />").arg(info.description());

	html += "<blockoute>";
	foreach (const PersonInfo &person, info.authors()) {
		html += "<br/>";
		html += tr("<b>Name:</b> %1<br/>").arg(person.name());
		if ( !person.task().toString().isEmpty() )
			html += tr("<b>Task:</b> %1<br/>").arg(person.task());
		if ( !person.email().isEmpty() )
			html += tr("<b>Email:</b> <a href=\"mailto:%1\">%1</a><br/>").arg(person.email());
		if ( !person.web().isEmpty() )
			html += tr("<b>Webpage:</b> <a href=\"%1\">%1</a><br/>").arg(person.web());
	}
	html += "</blockoute>";
	return html;
}

}


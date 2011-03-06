/****************************************************************************
 *  servicechooser.cpp
 *
 *  Copyright (c) 2010 by Aleksey Sidorov <sauron@citadelspb.com>
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

#include "servicechooser.h"
#include "servicechooserwidget.h"
#include "protocolchooserwidget.h"
#include "pluginchooserwidget.h"
#include <qutim/settingslayer.h>
#include <qutim/icon.h>
#include <qutim/extensioninfo.h>

namespace Core
{
ServiceChooser::ServiceChooser(QObject* parent): QObject(parent)
{
	GeneralSettingsItem<ServiceChooserWidget> *item = new GeneralSettingsItem<ServiceChooserWidget>(Settings::Plugin, Icon("applications-system"), QT_TRANSLATE_NOOP("Settings","Service chooser"));
	Settings::registerItem(item);
	GeneralSettingsItem<ProtocolChooserWidget> *item2 = new GeneralSettingsItem<ProtocolChooserWidget>(Settings::Protocol, Icon("applications-system"), QT_TRANSLATE_NOOP("Settings","Protocol chooser"));
	Settings::registerItem(item2);
	GeneralSettingsItem<PluginChooserWidget> *item3 = new GeneralSettingsItem<PluginChooserWidget>(Settings::Plugin, Icon("applications-system"), QT_TRANSLATE_NOOP("Settings","Plugin chooser"));
	Settings::registerItem(item3);
	deleteLater();
}

const char *ServiceChooser::className(const ExtensionInfo &info)
{
	return info.generator()->metaObject()->className();
}

QString ServiceChooser::html(const qutim_sdk_0_3::ExtensionInfo& info)
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

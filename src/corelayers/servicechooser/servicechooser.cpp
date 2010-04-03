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
#include "modulemanagerimpl.h"
#include "servicechooserwidget.h"
#include "protocolchooserwidget.h"
#include <libqutim/settingslayer.h>
#include <libqutim/icon.h>
#include <libqutim/extensioninfo.h>

namespace Core
{
	static CoreModuleHelper<ServiceChooser, StartupModule> service_shooser_static(
			QT_TRANSLATE_NOOP("Plugin", "Service chooser"),
			QT_TRANSLATE_NOOP("Plugin", "TODO")
			);

	ServiceChooser::ServiceChooser(QObject* parent): QObject(parent)
	{
		GeneralSettingsItem<ServiceChooserWidget> *item = new GeneralSettingsItem<ServiceChooserWidget>(Settings::Plugin, Icon("applications-system"), QT_TRANSLATE_NOOP("Settings","Service Chooser"));
		Settings::registerItem(item);
		GeneralSettingsItem<ProtocolChooserWidget> *item2 = new GeneralSettingsItem<ProtocolChooserWidget>(Settings::Protocol, Icon("applications-system"), QT_TRANSLATE_NOOP("Settings","Protocol chooser"));
		Settings::registerItem(item2);
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
			html += tr("<b>Name:</b> %1</br>").arg(person.name());
			html += tr("<b>Task:</b> %1</br>").arg(person.task());
			html += tr("<b>Email:</b> <a href=\"mailto:%1\">%1</a></br>").arg(person.email());
			html += tr("<b>Webpage:</b> <a href=\"%1\">%1</a></br>").arg(person.web());
		}
		html += "</blockoute>";
		return html;
	}	

}

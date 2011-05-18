/****************************************************************************
 *  authdialogplugin.cpp
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
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

#include "authdialogplugin.h"
#include "authservice.h"

namespace Core {

using namespace qutim_sdk_0_3;

void AuthDialogPlugin::init()
{
	qutim_sdk_0_3::ExtensionIcon icon(QLatin1String(""));
	qutim_sdk_0_3::LocalizedString name = QT_TRANSLATE_NOOP("Plugin", "Authorization dialog");
	qutim_sdk_0_3::LocalizedString description = QT_TRANSLATE_NOOP("Plugin", "Simple authorization dialog");
	setInfo(name, description, QUTIM_VERSION, icon);
	addAuthor(QT_TRANSLATE_NOOP("Author","Sidorov Aleksey"),
			  QT_TRANSLATE_NOOP("Task","Author"),
			  QLatin1String("sauron@citadelspb.com"),
			  QLatin1String("sauron.me"));
	addExtension<Core::AuthService>(name, description, icon);
}

bool AuthDialogPlugin::load()
{
	return true;
}

bool AuthDialogPlugin::unload()
{
	return true;
}

} // namespace Core

QUTIM_EXPORT_PLUGIN(Core::AuthDialogPlugin)

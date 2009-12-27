/****************************************************************************
 *  oscarplugin.cpp
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#include "oscarplugin.h"
#include "icqprotocol.h"
#include "ui/accountcreator.h"
#include <qutim/debug.h>

namespace Icq {

OscarPlugin::OscarPlugin()
{
	debug() << Q_FUNC_INFO;
}

void OscarPlugin::init()
{
	debug() << Q_FUNC_INFO;
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Oscar"),
			QT_TRANSLATE_NOOP("Plugin", "Module-based realization of Oscar protocol"),
			PLUGIN_VERSION(0, 0, 1, 0));
	addAuthor(QT_TRANSLATE_NOOP("Author", "Ruslan Nigmatullin"),
			  QT_TRANSLATE_NOOP("Task", "Author"),
			  QLatin1String("euroelessar@gmail.com"));
	addExtension(QT_TRANSLATE_NOOP("Plugin", "ICQ"),
				 QT_TRANSLATE_NOOP("Plugin", "Module-based realization of ICQ protocol"),
				 new GeneralGenerator<IcqProtocol>(),
				 ExtensionIcon("im-icq"));
	addExtension(QT_TRANSLATE_NOOP("Plugin", "ICQ account creator"),
				 QT_TRANSLATE_NOOP("Plugin", "Account creator for module-based realization of ICQ protocol"),
				 new GeneralGenerator<IcqAccountCreationWizard>(),
				 ExtensionIcon("im-icq"));
	Settings::registerItem(new GeneralSettingsItem<IcqMainSettings>(
						 Settings::Protocol,
						 QIcon(""),
						 QT_TRANSLATE_NOOP_UTF8("Settings", "Icq")));
}

bool OscarPlugin::load()
{
	return true;
}

bool OscarPlugin::unload()
{
	return false;
}

} // namespace Icq

QUTIM_EXPORT_PLUGIN(Icq::OscarPlugin)

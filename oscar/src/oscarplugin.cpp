/****************************************************************************
 *  oscarplugin.cpp
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *                        Prokhin Alexey <alexey.prokhin@yandex.ru>
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
#include <metainfo_p.h>
#include "ui/accountcreator.h"
#include "filetransfer.h"
#include <qutim/icon.h>
#include <qutim/debug.h>

namespace qutim_sdk_0_3 {

namespace oscar {

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
	addAuthor(QT_TRANSLATE_NOOP("Author", "Alexey Prokhin"),
			  QT_TRANSLATE_NOOP("Task", "Author"),
			  QLatin1String("alexey.prokhin@yandex.ru"));
	addExtension(QT_TRANSLATE_NOOP("Plugin", "ICQ"),
				 QT_TRANSLATE_NOOP("Plugin", "Module-based realization of ICQ protocol"),
				 new GeneralGenerator<IcqProtocol>(),
				 ExtensionIcon("im-icq"));
	addExtension(QT_TRANSLATE_NOOP("Plugin", "ICQ account creator"),
				 QT_TRANSLATE_NOOP("Plugin", "Account creator for module-based realization of ICQ protocol"),
				 new GeneralGenerator<IcqAccountCreationWizard>(),
				 ExtensionIcon("im-icq"));
	addExtension(QT_TRANSLATE_NOOP("Plugin", "Oscar file transfer protocol"),
				 QT_TRANSLATE_NOOP("Plugin", "Oscar file transfer protocol"),
				 new GeneralGenerator<OftFileTransferFactory, MessagePlugin>(),
				 ExtensionIcon("im-icq"));
	addExtension(QT_TRANSLATE_NOOP("Plugin", "MetaInfo icq service"),
				 QT_TRANSLATE_NOOP("Plugin", "MetaInfo icq service"),
				 new SingletonGenerator<MetaInfo, SNACHandler>(),
				 ExtensionIcon("im-icq"));
}

bool OscarPlugin::load()
{
	return true;
}

bool OscarPlugin::unload()
{
	return false;
}

} } // namespace qutim_sdk_0_3::oscar

QUTIM_EXPORT_PLUGIN(qutim_sdk_0_3::oscar::OscarPlugin)

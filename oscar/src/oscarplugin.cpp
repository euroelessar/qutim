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
#include "roster_p.h"
#include "metainfo/metainfo_p.h"
#include "messages_p.h"
#include "authorization_p.h"
#include "privacylists_p.h"
#include "ui/accountcreator.h"
#include "filetransfer.h"
#include "contactsearch_p.h"
#include "oscarproxy.h"
#include <qutim/icon.h>
#include <qutim/debug.h>
#include "ui/icqmainsettings.h"
#include <qutim/settingslayer.h>

namespace qutim_sdk_0_3 {

namespace oscar {

OscarPlugin::OscarPlugin()
{
}

void OscarPlugin::init()
{
	qRegisterMetaTypeStreamOperators<FeedbagItem>("qutim_sdk_0_3::oscar::FeedbagItem");
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Oscar"),
			QT_TRANSLATE_NOOP("Plugin", "Module-based implementation of Oscar protocol"),
			PLUGIN_VERSION(0, 0, 1, 0));
	addAuthor(QT_TRANSLATE_NOOP("Author", "Ruslan Nigmatullin"),
			  QT_TRANSLATE_NOOP("Task", "Author"),
			  QLatin1String("euroelessar@gmail.com"));
	addAuthor(QT_TRANSLATE_NOOP("Author", "Alexey Prokhin"),
			  QT_TRANSLATE_NOOP("Task", "Author"),
			  QLatin1String("alexey.prokhin@yandex.ru"));
	addExtension(QT_TRANSLATE_NOOP("Plugin", "ICQ"),
				 QT_TRANSLATE_NOOP("Plugin", "Module-based implementation of ICQ protocol"),
				 new GeneralGenerator<IcqProtocol>(),
				 ExtensionIcon("im-icq"));
	addExtension(QT_TRANSLATE_NOOP("Plugin", "ICQ account creator"),
				 QT_TRANSLATE_NOOP("Plugin", "Account creator for module-based implementation of ICQ protocol"),
				 new GeneralGenerator<IcqAccountCreationWizard>(),
				 ExtensionIcon("im-icq"));
	addExtension(QT_TRANSLATE_NOOP("Plugin", "Oscar roster"),
				 QT_TRANSLATE_NOOP("Plugin", "Oscar roster"),
				 new SingletonGenerator<Roster, SNACHandler, FeedbagItemHandler>(),
				 ExtensionIcon("im-icq"));
	addExtension(QT_TRANSLATE_NOOP("Plugin", "Oscar messages"),
				 QT_TRANSLATE_NOOP("Plugin", "Oscar messages"),
				 new SingletonGenerator<MessagesHandler, SNACHandler>(),
				 ExtensionIcon("im-icq"));
	addExtension(QT_TRANSLATE_NOOP("Plugin", "Oscar file transfer protocol"),
				 QT_TRANSLATE_NOOP("Plugin", "Oscar file transfer protocol"),
				 new GeneralGenerator<OftFileTransferFactory, MessagePlugin>(),
				 ExtensionIcon("im-icq"));
	addExtension(QT_TRANSLATE_NOOP("Plugin", "MetaInfo icq service"),
				 QT_TRANSLATE_NOOP("Plugin", "MetaInfo icq service"),
				 new SingletonGenerator<MetaInfo, SNACHandler>(),
				 ExtensionIcon("im-icq"));
	addExtension(QT_TRANSLATE_NOOP("Plugin", "Oscar authorization support"),
				 QT_TRANSLATE_NOOP("Plugin", "Oscar authorization support"),
				 new SingletonGenerator<Authorization, SNACHandler, FeedbagItemHandler>(),
				 ExtensionIcon("im-icq"));
	addExtension(QT_TRANSLATE_NOOP("Plugin", "Oscar privacy lists"),
				 QT_TRANSLATE_NOOP("Plugin", "Oscar privacy lists"),
				 new SingletonGenerator<PrivacyLists, FeedbagItemHandler>(),
				 ExtensionIcon("im-icq"));
	addExtension(QT_TRANSLATE_NOOP("Plugin", "Oscar contact search"),
				 QT_TRANSLATE_NOOP("Plugin", "Oscar contact search implementation"),
				 new GeneralGenerator<OscarContactSearchFactory>(),
				 ExtensionIcon("im-icq"));
	addExtension(QT_TRANSLATE_NOOP("Plugin", "Oscar proxy support"),
				 QT_TRANSLATE_NOOP("Plugin", "Oscar proxy support"),
				 new SingletonGenerator<OscarProxyManager, NetworkProxyManager>(),
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

QUTIM_EXPORT_PLUGIN2(oscar,qutim_sdk_0_3::oscar::OscarPlugin)

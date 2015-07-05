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

#include "oscarplugin.h"
#include "icqprotocol.h"
#include "oscarroster_p.h"
#include "metainfo/metainfo_p.h"
#include "messages_p.h"
#include "authorization_p.h"
#include "privacylists_p.h"
#include "ui/accountcreator.h"
#include "oscarfiletransfer_p.h"
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
	addAuthor(QLatin1String("euroelessar"));
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
				 new SingletonGenerator<OftFileTransferFactory, MessagePlugin>(),
				 ExtensionIcon("im-icq"));
	addExtension(QT_TRANSLATE_NOOP("Plugin", "Oscar file transfer settings"),
				 QT_TRANSLATE_NOOP("Plugin", "Oscar file transfer settings"),
				 new SingletonGenerator<OscarFileTransferSettings, SettingsExtension>(),
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


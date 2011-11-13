/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#include "ircplugin.h"
#include "ircprotocol.h"
#include "ircproxymanager.h"
#include "ui/accountcreator.h"

namespace qutim_sdk_0_3 {

namespace irc {

IrcPlugin::IrcPlugin()
{
}

void IrcPlugin::init()
{
	setInfo(QT_TRANSLATE_NOOP("Plugin", "IRC"),
			QT_TRANSLATE_NOOP("Plugin", "Implementation of IRC protocol"),
			PLUGIN_VERSION(0, 0, 1, 0));
	addAuthor(QT_TRANSLATE_NOOP("Author", "Alexey Prokhin"),
			  QT_TRANSLATE_NOOP("Task", "Author"),
			  QLatin1String("alexey.prokhin@yandex.ru"));
	addExtension(QT_TRANSLATE_NOOP("Plugin", "IRC"),
				 QT_TRANSLATE_NOOP("Plugin", "Implementation of IRC protocol"),
				 new GeneralGenerator<IrcProtocol>(),
				 ExtensionIcon("im-irc"));
	addExtension(QT_TRANSLATE_NOOP("Plugin", "IRC account creator"),
				 QT_TRANSLATE_NOOP("Plugin", "Account creator for IRC protocol"),
				 new GeneralGenerator<IrcAccountCreationWizard>(),
				 ExtensionIcon("im-irc"));
	addExtension(QT_TRANSLATE_NOOP("Plugin", "Proxy support"),
				 QT_TRANSLATE_NOOP("Plugin", "Proxy support for IRC protocol"),
				 new GeneralGenerator<IrcProxyManager, qutim_sdk_0_3::NetworkProxyManager>(),
				 ExtensionIcon("im-irc"));
}

bool IrcPlugin::load()
{
	return true;
}

bool IrcPlugin::unload()
{
	return false;
}

QString qutimIrcVersionStr()
{
	return QUTIM_IRC_VERSION_STR;
}

} } // namespace qutim_sdk_0_3::irc

QUTIM_EXPORT_PLUGIN2(irc, qutim_sdk_0_3::irc::IrcPlugin)


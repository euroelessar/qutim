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

#include "vkontakteplugin.h"
#include "ui/vaccountcreator.h"
#include "vaccount.h"
#include "vproxymanager.h"
#include "vprotocol.h"

#include <qutim/debug.h>
#include <qutim/settingslayer.h>
#include <qutim/menucontroller.h>
#include <qutim/icon.h>


void VkontaktePlugin::init()
{
	debug() << Q_FUNC_INFO;
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Vkontakte"),
			QT_TRANSLATE_NOOP("Plugin", "Simple implementation of vkontakte, based on vk.com/developers api"),
			PLUGIN_VERSION(1, 6, 0, 0));
	addAuthor(QLatin1String("sauron"));

	ExtensionIcon vicon = ExtensionIcon("im-vkontakte");

	addExtension(QT_TRANSLATE_NOOP("Plugin", "vkontakte"),
				 QT_TRANSLATE_NOOP("Plugin", "Simple implementation of vkontakte, based on desktop.api"),
				 new GeneralGenerator<VProtocol>(),
				 vicon
				 );
	addExtension(QT_TRANSLATE_NOOP("Plugin", "Vkontakte account creator"),
				 QT_TRANSLATE_NOOP("Plugin", "Account creator for Vkontakte"),
				 new GeneralGenerator<VAccountCreator>(),
				 vicon);
	addExtension(QT_TRANSLATE_NOOP("Plugin", "Proxy support"),
				 QT_TRANSLATE_NOOP("Plugin", "Proxy support for Vkontakte"),
				 new GeneralGenerator<VProxyManager, NetworkProxyManager>(),
				 vicon);
}
bool VkontaktePlugin::load()
{
	return true;
}
bool VkontaktePlugin::unload()
{
	return false;
}

QUTIM_EXPORT_PLUGIN(VkontaktePlugin)


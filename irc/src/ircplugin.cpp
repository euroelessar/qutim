/****************************************************************************
 *  ircplugin.cpp
 *
 *  Copyright (c) 2010 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#include "ircplugin.h"
#include "ircprotocol.h"
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
	addExtension(QT_TRANSLATE_NOOP("Plugin", "ICQ account creator"),
				 QT_TRANSLATE_NOOP("Plugin", "Account creator for IRC protocol"),
				 new GeneralGenerator<IrcAccountCreationWizard>(),
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

} } // namespace qutim_sdk_0_3::irc

QUTIM_EXPORT_PLUGIN2(irc, qutim_sdk_0_3::irc::IrcPlugin)

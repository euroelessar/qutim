/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#include "chatlayerplugin.h"
#include "chatlayer/chatlayerimpl.h"
#include "adiumchatform/adiumchatform.h"
#include "classicchatform/classicchatform.h"
#include "timemodifier/timemodifier.h"
#include "settings/chatbehavior.h"
#include <qutim/settingslayer.h>
#include <qutim/icon.h>
#include <qutim/servicemanager.h>
#include "webkitchat/webkitviewfactory.h"

namespace Core
{
namespace AdiumChat
{

static SettingsItem *behaviourSettings = 0;

using namespace qutim_sdk_0_3;

void ChatLayerPlugin::init()
{
	LocalizedString name = QT_TRANSLATE_NOOP("Plugin", "Webkit chat layer");
	LocalizedString description = QT_TRANSLATE_NOOP("Plugin", "Default qutIM chat implementation, based on Adium chat styles");
	setInfo(name, description, QUTIM_VERSION);
	addExtension<Core::AdiumChat::ChatLayerImpl>(name, description);
	addExtension<Core::AdiumChat::AdiumChatForm>(
				QT_TRANSLATE_NOOP("Plugin", "Adium"),
				QT_TRANSLATE_NOOP("Plugin", "Adium-like chatform implementation for adiumchat"));
	addExtension<Core::AdiumChat::TimeModifier,Core::AdiumChat::MessageModifier>(
				QT_TRANSLATE_NOOP("Plugin", "Time modifier"),
				QT_TRANSLATE_NOOP("Plugin", "Modifier for %time% and %timeOpened% param, based on Apple's documentation"));
	addExtension<Core::AdiumChat::ClassicChatForm>(
				QT_TRANSLATE_NOOP("Plugin", "Classic"),
				QT_TRANSLATE_NOOP("Plugin", "Default qutIM chatform implementation for adiumchat"));
	addExtension<Core::AdiumChat::WebkitViewFactory>(
				QT_TRANSLATE_NOOP("Plugin", "Webkit"),
				QT_TRANSLATE_NOOP("Plugin", "Default qutIM chat implementation, based on Adium chat styles"));
}

bool ChatLayerPlugin::load()
{
	QObject *obj = ServiceManager::getByName("ChatForm");
	if (!obj) {
		return false;
	}

	behaviourSettings = new GeneralSettingsItem<ChatBehavior>(Settings::General, Icon("view-choose"),
															  QT_TRANSLATE_NOOP("Settings","Chat"));
	behaviourSettings->connect(SIGNAL(saved()), obj, SLOT(onBehaviorSettingsChanged()));
	Settings::registerItem(behaviourSettings);
	return true;
}

bool ChatLayerPlugin::unload()
{
	Settings::removeItem(behaviourSettings);
	delete behaviourSettings;
	return true;
}

}
}

QUTIM_EXPORT_PLUGIN(Core::AdiumChat::ChatLayerPlugin)

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
#include "settings/chatsettings.h"

namespace Core
{
	namespace AdiumChat
	{
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
			addExtension<Core::ChatSettings,StartupModule>(
					QT_TRANSLATE_NOOP("Plugin", "Chat settings"),
					QT_TRANSLATE_NOOP("Plugin", ""));
			addExtension<Core::AdiumChat::TimeModifier,Core::AdiumChat::MessageModifier>(
					QT_TRANSLATE_NOOP("Plugin", "Time modifier"),
					QT_TRANSLATE_NOOP("Plugin", "Modifier for %time% and %timeOpened% param, based on Apple's documentation"));
			addExtension<Core::AdiumChat::ClassicChatForm>(
					QT_TRANSLATE_NOOP("Plugin", "Classic"),
					QT_TRANSLATE_NOOP("Plugin", "Default qutIM chatform implementation for adiumchat"));
		}
		
		bool ChatLayerPlugin::load()
		{
			return true;
		}
		
		bool ChatLayerPlugin::unload()
		{
			return true;
		}
	}
}

QUTIM_EXPORT_PLUGIN(Core::AdiumChat::ChatLayerPlugin)

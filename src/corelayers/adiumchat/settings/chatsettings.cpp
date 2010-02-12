#include "chatsettings.h"
#include "chatappearance.h"
#include "../chatlayerimpl.h"
#include "modulemanagerimpl.h"
#include <libqutim/settingslayer.h>
#include <libqutim/icon.h>

namespace Core
{
	static CoreModuleHelper<ChatSettings, StartupModule> chat_settings_static(
			QT_TRANSLATE_NOOP("Plugin", "Chat settings"),
			QT_TRANSLATE_NOOP("Plugin", "Default settings widgets for adium chat")
			);
	
	ChatSettings::ChatSettings()
	{
		GeneralSettingsItem<ChatAppearance> *item = new GeneralSettingsItem<ChatAppearance>(Settings::Appearance, Icon("view-choose"), QT_TRANSLATE_NOOP("Settings","Chat appearance"));
		Settings::registerItem(item);
		deleteLater();
	}

}

#include "chatsettings.h"
#include "chatappearance.h"
#include "chatbehavior.h"
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
		QObject *obj = getService("ChatForm");
		if (!obj) {
			deleteLater();
			return;
		}
		SettingsItem *item;
		item = new GeneralSettingsItem<ChatAppearance>(Settings::Appearance, Icon("view-choose"),
													   QT_TRANSLATE_NOOP("Settings","Chat"));
		item->connect(SIGNAL(saved()), obj, SLOT(onAppearanceSettingsChanged()));
		Settings::registerItem(item);
		item = new GeneralSettingsItem<ChatBehavior>(Settings::General, Icon("view-choose"),
													 QT_TRANSLATE_NOOP("Settings","Chat"));
		item->connect(SIGNAL(saved()), obj, SLOT(onBehaviorSettingsChanged()));
		Settings::registerItem(item);
		deleteLater();
	}

}

#include "popupsettings.h"
#include "modulemanagerimpl.h"
#include "popupappearance.h"
#include "popupbehavior.h"
#include <libqutim/settingslayer.h>
#include <libqutim/icon.h>
#include <QDebug>
#include "../backend.h"

namespace Core
{
	static CoreModuleHelper<PopupSettings, StartupModule> popups_settings_static(
		QT_TRANSLATE_NOOP("Plugin", "Notifications settings"),
		QT_TRANSLATE_NOOP("Plugin", "Default settings widgets for kinetic notifications")
		);
	PopupSettings::PopupSettings()
	{
 		GeneralSettingsItem<PopupAppearance> *appearance = new GeneralSettingsItem<PopupAppearance>(Settings::Appearance, Icon("dialog-information"), QT_TRANSLATE_NOOP("Settings","Popups"));
		GeneralSettingsItem<PopupBehavior> *behavior = new GeneralSettingsItem<PopupBehavior>(Settings::General,Icon("dialog-information"),QT_TRANSLATE_NOOP("Settings","Notifications"));
 		Settings::registerItem(appearance);
		Settings::registerItem(behavior);
	}

}
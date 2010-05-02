#include "shortcutsettings.h"
#include "modulemanagerimpl.h"
#include "shortcutsettingswidget.h"
#include <libqutim/settingslayer.h>
#include <libqutim/icon.h>

namespace Core
{
	static CoreModuleHelper<ShortcutSettings, StartupModule> shortcut_settings_static(
			QT_TRANSLATE_NOOP("Plugin", "Shortcut settings"),
			QT_TRANSLATE_NOOP("Plugin", "Shortcut editor")
			);

	ShortcutSettings::ShortcutSettings(QObject *parent) :
			QObject(parent)
	{
		GeneralSettingsItem<ShortcutSettingsWidget> *item = new GeneralSettingsItem<ShortcutSettingsWidget>(Settings::General, Icon("preferences-desktop-peripherals"), QT_TRANSLATE_NOOP("Settings","Shortcuts"));
		Settings::registerItem(item);
		deleteLater();
	}

}

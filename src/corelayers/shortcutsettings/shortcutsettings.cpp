#include "shortcutsettings.h"
#include "shortcutsettingswidget.h"
#include <libqutim/settingslayer.h>
#include <libqutim/icon.h>

namespace Core
{
	ShortcutSettings::ShortcutSettings(QObject *parent) :
			QObject(parent)
	{
		GeneralSettingsItem<ShortcutSettingsWidget> *item = new GeneralSettingsItem<ShortcutSettingsWidget>(Settings::General, Icon("preferences-desktop-peripherals"), QT_TRANSLATE_NOOP("Settings","Shortcuts"));
		Settings::registerItem(item);
		deleteLater();
	}

}

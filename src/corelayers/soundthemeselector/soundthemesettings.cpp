#include "soundthemesettings.h"
#include "modulemanagerimpl.h"
#include "soundthemeselector.h"
#include <libqutim/settingslayer.h>
#include <libqutim/icon.h>

namespace Core
{
	static CoreModuleHelper<SoundThemeSettings, StartupModule> sound_theme_settings_static(
			QT_TRANSLATE_NOOP("Plugin", "Sound theme selector"),
			QT_TRANSLATE_NOOP("Plugin", "Simple sound theme selector")
			);

	SoundThemeSettings::SoundThemeSettings(QObject *parent) :
			QObject(parent)
	{
		GeneralSettingsItem<SoundThemeSelector> *item = new GeneralSettingsItem<SoundThemeSelector>(Settings::Appearance, Icon("speaker"), QT_TRANSLATE_NOOP("Settings","Sounds"));
		Settings::registerItem(item);
		deleteLater();
	}

}

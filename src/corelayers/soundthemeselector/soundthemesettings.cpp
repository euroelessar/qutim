#include "soundthemesettings.h"
#include "soundthemeselector.h"
#include <qutim/settingslayer.h>
#include <qutim/icon.h>

namespace Core
{
	SoundThemeSettings::SoundThemeSettings(QObject *parent) :
			QObject(parent)
	{
		GeneralSettingsItem<SoundThemeSelector> *item = new GeneralSettingsItem<SoundThemeSelector>(Settings::Appearance, Icon("speaker"), QT_TRANSLATE_NOOP("Settings","Sounds"));
		Settings::registerItem(item);
		deleteLater();
	}

}

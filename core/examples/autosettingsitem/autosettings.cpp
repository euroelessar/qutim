#include "autosettings.h"
#include  "modulemanagerimpl.h" 
#include <libqutim/settingslayer.h>
#include <QCheckBox>

namespace Core
{
static CoreModuleHelper<AutoSettings, StartupModule> autosettings_settings_static(
	QT_TRANSLATE_NOOP("Plugin", "Autosettings example"),
	QT_TRANSLATE_NOOP("Plugin", "Simple example of auto settings item generator")
	);

AutoSettings::AutoSettings()
{
	AutoSettingsItem *item = new AutoSettingsItem(Settings::General, QT_TRANSLATE_NOOP("Settings", "Test settings"));
	item->setConfig(QString(), "test");
	item->addEntry<QLineEdit>(QT_TRANSLATE_NOOP("Settings", "Text"))->setName("text")->setProperty("acceptRichText", false);
	item->addEntry<AutoSettingsComboBox>(QT_TRANSLATE_NOOP("Settings", "Combo"))->setName("combo")->setProperty("items", QStringList() << "First" << "Second" << "Third");
	item->addEntry<QCheckBox>(QT_TRANSLATE_NOOP("Settings", "Check"))->setName("check");
	Settings::registerItem(item);
}
}
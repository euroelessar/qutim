#include "testplugin.h"
#include "testsettings.h"
#include <qutim/config.h>
#include <qutim/settingslayer.h>
#include <QMessageBox>

using namespace qutim_sdk_0_3;

void TestPlugin::init()
{
	addAuthor(QT_TRANSLATE_NOOP("Author", "Vizir Ivan"),
			QT_TRANSLATE_NOOP("Task", "Author"),
			QLatin1String("define-true-false@yandex.com"));
	setInfo(QT_TRANSLATE_NOOP("Plugin", "TestPlugin"), // название
			QT_TRANSLATE_NOOP("Plugin", "A plugin made for test purposes only."), // описание
			PLUGIN_VERSION(1, 0, 0, 0), // версия
			ExtensionIcon()); // значок
	settingsitem = new GeneralSettingsItem<TestSettings>(
				Settings::Plugin,	QIcon(),
				QT_TRANSLATE_NOOP("Plugin", "Test Plugin"));
}

bool TestPlugin::load()
{
	Settings::registerItem(settingsitem);
	Config cfg("testplugin");
	if (cfg.value("showmsgbox", false))
		QMessageBox::information(0, "Test Plugin", "Test plugin loaded. Cool?");
	return true;
}

bool TestPlugin::unload()
{
	Settings::removeItem(settingsitem);
	Config cfg("testplugin");
	if (cfg.value("showmsgbox", false))
		QMessageBox::information(0, "Test Plugin", "Test plugin unloaded. Cool?");
	return true;
}

QUTIM_EXPORT_PLUGIN(TestPlugin)

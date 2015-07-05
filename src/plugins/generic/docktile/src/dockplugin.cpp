#include "dockplugin.h"

void DockPlugin::init()
{
	qutim_sdk_0_3::ExtensionIcon icon("info");
	addAuthor(QT_TRANSLATE_NOOP("Author", "Aleksey Sidorov"),
			QT_TRANSLATE_NOOP("Task", "Developer"),
			QLatin1String("gorthauer87@ya.ru")
			);
	setInfo(QT_TRANSLATE_NOOP("Plugin", "DockTile"),
			QT_TRANSLATE_NOOP("Plugin", "Dock tile integration"),
			PLUGIN_VERSION(0, 0, 1, 0),
			icon
			);
	setCapabilities(Loadable);
}

bool DockPlugin::load()
{
	m_dockTile = new DockTile(this);
	return true;
}

bool DockPlugin::unload()
{
	m_dockTile->deleteLater();
	return true;
}

QUTIM_EXPORT_PLUGIN(DockPlugin)

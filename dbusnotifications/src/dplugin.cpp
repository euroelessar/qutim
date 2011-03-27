#include "dplugin.h"
#include "dbusbackend.h"


DPlugin::DPlugin()
{
	qDebug("%s", Q_FUNC_INFO);
}

void DPlugin::init()
{
	qDebug("%s", Q_FUNC_INFO);
	ExtensionIcon dIcon("mac");
	setInfo(QT_TRANSLATE_NOOP("Plugin", "DBus notifications"),
		QT_TRANSLATE_NOOP("Plugin", "Notification system based on Freedesktop DBus protocol"),
		PLUGIN_VERSION(0, 0, 1, 0),
		dIcon);
	addAuthor(QT_TRANSLATE_NOOP("Author","Sidorov Aleksey"),
			  QT_TRANSLATE_NOOP("Task","Author"),
			  QLatin1String("sauron@citadelspb.com"),
			  QLatin1String("sauron.me"));
	addExtension<DBusBackend>(QT_TRANSLATE_NOOP("plugin","DBus notifications"),
							   QT_TRANSLATE_NOOP("plugin","Notification system based on Freedesktop DBus protocol"));
}

bool DPlugin::load()
{
	return true;
}

bool DPlugin::unload()
{
	return false;
}


QUTIM_EXPORT_PLUGIN(DPlugin)

#include "mplugin.h"
#include "dock/mdock.h"

namespace MacIntegration
{
	MPlugin::MPlugin()
	{
		qDebug("%s", Q_FUNC_INFO);
	}

	void MPlugin::init()
	{
		qDebug("%s", Q_FUNC_INFO);
		ExtensionIcon macIcon("mac");
		setInfo(QT_TRANSLATE_NOOP("Plugin", "MacIntegration"),
			QT_TRANSLATE_NOOP("Plugin", "Mac integration plugin collection"),
			PLUGIN_VERSION(0, 0, 1, 0),
			macIcon);
		addAuthor(QT_TRANSLATE_NOOP("Author", "Denis Daschenko"),
			QT_TRANSLATE_NOOP("Task", "Author"),
			QLatin1String("daschenko@gmail.com"));
		addExtension<MDock>(QT_TRANSLATE_NOOP("Plugin", "DockIntegration"),
				QT_TRANSLATE_NOOP("Plugin", "Mac OS X dock integration"));
	}

	bool MPlugin::load()
	{
		return true;
	}

	bool MPlugin::unload()
	{
		return false;
	}

} // MacIntegration namespace

QUTIM_EXPORT_PLUGIN(MacIntegration::MPlugin)

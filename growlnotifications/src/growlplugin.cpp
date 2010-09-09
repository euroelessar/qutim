#include "growlplugin.h"
#include "growlbackend.h"
#include <qutim/debug.h>

using namespace qutim_sdk_0_3;

void GrowlPlugin::init()
{
	//TODO add icon
	debug() << Q_FUNC_INFO;
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Growl notifications"),
			QT_TRANSLATE_NOOP("Plugin", "Default Mac Os X Notifications"),
			PLUGIN_VERSION(0, 1, 0, 0));
	addAuthor(QT_TRANSLATE_NOOP("Author","Aleksey Sidorov"),
			  QT_TRANSLATE_NOOP("Task","Author"),
			  QLatin1String("sauron@citadelspb.com"),
			  QLatin1String("sauron.me"));
	addExtension<GrowlBackend>(QT_TRANSLATE_NOOP("plugin","Growl notifications"),
							   QT_TRANSLATE_NOOP("plugin","Default Mac Os X Notifications"));
}

bool GrowlPlugin::load()
{
	return true;
}

bool GrowlPlugin::unload()
{
	return false;
}

QUTIM_EXPORT_PLUGIN(GrowlPlugin)

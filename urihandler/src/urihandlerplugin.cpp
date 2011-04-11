#include "urihandlerservice.h"
#include "urihandlerplugin.h"

#include <QUrl>

#include <qutim/debug.h>

void UriHandlerPlugin::init()
{
	addAuthor(QT_TRANSLATE_NOOP("Author", "Vizir Ivan"),
		QT_TRANSLATE_NOOP("Task", "Author"),
		QLatin1String("define-true-false@yandex.com"));
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Uri Handler"),
		QT_TRANSLATE_NOOP("Plugin", "Allows qutIM to open link of some types."),
		PLUGIN_VERSION(1, 0, 0, 0));
	addExtension<UriHandlerService>("UriHandler", "Allows qutIM to open link of some types.");
}

bool UriHandlerPlugin::load()
{
	return true;
}

bool UriHandlerPlugin::unload()
{
	return true;
}

QUTIM_EXPORT_PLUGIN(UriHandlerPlugin)

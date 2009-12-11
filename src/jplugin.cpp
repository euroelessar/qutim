#include "jplugin.h"
#include "jprotocol.h"

namespace Jabber {

	jPlugin::jPlugin()
	{
		qDebug("%s", Q_FUNC_INFO);
	}

	void jPlugin::init()
	{
		qDebug("%s", Q_FUNC_INFO);
		setInfo(QT_TRANSLATE_NOOP("Plugin", "Jabber"), QT_TRANSLATE_NOOP("Plugin", "Jabber protocol"), PLUGIN_VERSION(0, 0, 1, 0));
		addAuthor(QT_TRANSLATE_NOOP("Author", "Denis Daschenko"), QT_TRANSLATE_NOOP("Task", "Author"), QLatin1String("daschenko@gmail.com"));
		addExtension(QT_TRANSLATE_NOOP("Plugin", "Jabber"), QT_TRANSLATE_NOOP("Plugin", "Jabber protocol"), new GeneralGenerator<jProtocol>(), ExtensionIcon("im-jabber"));
	}

	bool jPlugin::load()
	{
		return true;
	}

	bool jPlugin::unload()
	{
		return false;
	}

} // Jabber namespace

QUTIM_EXPORT_PLUGIN(Jabber::jPlugin)

#include "jplugin.h"
#include "protocol/jprotocol.h"

namespace Jabber
{
	JPlugin::JPlugin()
	{
		qDebug("%s", Q_FUNC_INFO);
	}

	void JPlugin::init()
	{
		qDebug("%s", Q_FUNC_INFO);
		setInfo(QT_TRANSLATE_NOOP("Plugin", "Jabber"),
				QT_TRANSLATE_NOOP("Plugin", "Jabber protocol"),
				PLUGIN_VERSION(0, 0, 1, 0),
				ExtensionIcon("im-jabber"));
		addAuthor(QT_TRANSLATE_NOOP("Author", "Denis Daschenko"),
				  QT_TRANSLATE_NOOP("Task", "Author"),
				  QLatin1String("daschenko@gmail.com"));
		addExtension(QT_TRANSLATE_NOOP("Plugin", "Jabber"),
					 QT_TRANSLATE_NOOP("Plugin", "Jabber protocol"),
					 new GeneralGenerator<JProtocol>(),
					 ExtensionIcon("im-jabber"));
	}

	bool JPlugin::load()
	{
		return true;
	}

	bool JPlugin::unload()
	{
		return false;
	}

} // Jabber namespace

QUTIM_EXPORT_PLUGIN(Jabber::JPlugin)

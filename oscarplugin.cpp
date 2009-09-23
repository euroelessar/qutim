#include "oscarplugin.h"
#include "icqprotocol.h"

OscarPlugin::OscarPlugin()
{
	qDebug("%s", Q_FUNC_INFO);
}

void OscarPlugin::init()
{
	qDebug("%s", Q_FUNC_INFO);
	m_info = PluginInfo(QT_TRANSLATE_NOOP("Plugin", "Oscar"),
						QT_TRANSLATE_NOOP("Plugin", "Module-based realization of Oscar protocol"),
						QUTIM_VERSION_CHECK(0, 0, 1, 0));
	m_info.addAuthor(QT_TRANSLATE_NOOP("Author", "Ruslan Nigmatullin"),
					 QT_TRANSLATE_NOOP("Task", "Author"),
					 QLatin1String("euroelessar@gmail.com"));
	m_extensions << ExtensionInfo(QT_TRANSLATE_NOOP("Plugin", "ICQ"),
								  QT_TRANSLATE_NOOP("Plugin", "Module-based realization of ICQ protocol"),
								  new GeneralGenerator<IcqProtocol>());
}

bool OscarPlugin::load()
{
	return true;
}

bool OscarPlugin::unload()
{
	return false;
}

QUTIM_EXPORT_PLUGIN(OscarPlugin)

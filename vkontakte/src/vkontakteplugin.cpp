#include "vkontakteplugin.h"
#include <qutim/debug.h>
#include "vkontakteprotocol.h"

void VkontaktePlugin::init()
{
	debug() << Q_FUNC_INFO;
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Vkontakte"),
			QT_TRANSLATE_NOOP("Plugin", "Simple implementation of vkontakte, based on userapi.ru"),
			PLUGIN_VERSION(0, 0, 1, 0));
	addAuthor(QT_TRANSLATE_NOOP("Author", "Aleksey Sidorov"),
			  QT_TRANSLATE_NOOP("Task", "Author"),
			  QLatin1String("sauron@citadelspb.com"));
// 	addAuthor(QT_TRANSLATE_NOOP("Author", "Rustam Chakin"),
// 			  QT_TRANSLATE_NOOP("Task", "Founder"),
// 			  QLatin1String(""));

	addExtension(QT_TRANSLATE_NOOP("Plugin", "Userapi"),
				 QT_TRANSLATE_NOOP("Plugin", "Simple implementation of vkontakte, based on userapi.ru"),
				 new GeneralGenerator<VkontakteProtocol>(),
				 ExtensionIcon("im-vkontakte"));
	
}
bool VkontaktePlugin::load()
{
	return true;
}
bool VkontaktePlugin::unload()
{
	return false;
}

QUTIM_EXPORT_PLUGIN(VkontaktePlugin)
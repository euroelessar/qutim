#include "macplugin.h"
#include "dock/macdock.h"
#include "growl/growlbackend.h"
#include "idle/macidle.h"
#include "spellchecker/macspellchecker.h"

namespace MacIntegration
{
	MacPlugin::MacPlugin()
	{
		qDebug("%s", Q_FUNC_INFO);
	}

	void MacPlugin::init()
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
		addExtension<MacDock>(QT_TRANSLATE_NOOP("Plugin", "DockIntegration"),
				QT_TRANSLATE_NOOP("Plugin", "Mac OS X dock integration"));
		addExtension<GrowlBackend>(QT_TRANSLATE_NOOP("plugin","Growl notifications"),
				QT_TRANSLATE_NOOP("plugin","Default Mac Os X Notifications"));
		addExtension<MacIdle>(QT_TRANSLATE_NOOP("Plugin", "IdleManager"),
				QT_TRANSLATE_NOOP("Plugin", "Mac OS X idle manager"));
		addExtension<MacSpellChecker>(QT_TRANSLATE_NOOP("Plugin", "SpellChecker"),
				QT_TRANSLATE_NOOP("Plugin", "Mac OS X spell checker"));
	}

	bool MacPlugin::load()
	{
		return true;
	}

	bool MacPlugin::unload()
	{
		return false;
	}

} // MacIntegration namespace

QUTIM_EXPORT_PLUGIN(MacIntegration::MacPlugin)

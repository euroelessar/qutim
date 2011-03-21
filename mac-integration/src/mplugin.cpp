#include "mplugin.h"
#include "dock/mdock.h"
#include "growl/growlbackend.h"
#include "idle/midle.h"
#include "spellchecker/mspellchecker.h"
#include "menu/mmenu.h"

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
		addExtension<GrowlBackend>(QT_TRANSLATE_NOOP("plugin","Growl notifications"),
				QT_TRANSLATE_NOOP("plugin","Default Mac Os X Notifications"));
		addExtension<MIdle>(QT_TRANSLATE_NOOP("Plugin", "IdleManager"),
				QT_TRANSLATE_NOOP("Plugin", "Mac OS X idle manager"));
		addExtension<MSpellChecker>(QT_TRANSLATE_NOOP("Plugin", "SpellChecker"),
				QT_TRANSLATE_NOOP("Plugin", "Mac OS X spell checker"));
		addExtension<MMenu>(QT_TRANSLATE_NOOP("Plugin", "MainMenu"),
				QT_TRANSLATE_NOOP("Plugin", "Mac OS X main menu"));
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

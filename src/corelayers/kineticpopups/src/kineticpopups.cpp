#include "kineticpopups.h"
#include <qutim/debug.h>
#include "backend.h"
#include <qutim/icon.h>
#include "settings/popupappearance.h"
#include <qutim/settingslayer.h>
#include <qutim/icon.h>

namespace Core
{
namespace KineticPopups
{
void KineticPopupsPlugin::init()
{
	debug() << Q_FUNC_INFO;
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Kinetic popups"),
			QT_TRANSLATE_NOOP("Plugin", "Default qutIM popups based on QTextDocument"),
			PLUGIN_VERSION(0, 1, 0, 0));
	addAuthor(QT_TRANSLATE_NOOP("Author","Sidorov Aleksey"),
			  QT_TRANSLATE_NOOP("Task","Author"),
			  QLatin1String("sauron@citadelspb.com"),
			  QLatin1String("sauron.me"));
	addExtension<Backend>(QT_TRANSLATE_NOOP("plugin","Kinetic Popups"),QT_TRANSLATE_NOOP("plugin","Default qutIM popups based on QTextDocument"));
}

bool KineticPopupsPlugin::load()
{
//	GeneralSettingsItem<PopupAppearance> *appearance = new GeneralSettingsItem<PopupAppearance>(Settings::Appearance, Icon("dialog-information"), QT_TRANSLATE_NOOP("Settings","Popups"));
//	Settings::registerItem(appearance);
	return true;
}

bool KineticPopupsPlugin::unload()
{
	return false;
}

}
}

QUTIM_EXPORT_PLUGIN(Core::KineticPopups::KineticPopupsPlugin)

#include "applicationwindowplugin.h"
#include "applicationwindow.h"
#include "chat.h"
#include "contactlist.h"
#include "quickpassworddialog.h"
#include "quickauthdialog.h"

namespace MeegoIntegration
{
	void Plugin::init()
	{
		qutim_sdk_0_3::LocalizedString name = QT_TRANSLATE_NOOP("Plugin", "Meego application window");
		qutim_sdk_0_3::LocalizedString description = QT_TRANSLATE_NOOP("Plugin", "Main window of application");
		setInfo(name, description, PLUGIN_VERSION(0, 1, 0, 0));
		addExtension<ApplicationWindow>(name, description);
		addExtension<Chat>(
			    QT_TRANSLATE_NOOP("Plugin", "Meego chat page"),
			    QT_TRANSLATE_NOOP("Plugin", "Simple chat based on QML elements"));
		addExtension<ContactList>(
			    QT_TRANSLATE_NOOP("Plugin", "Meego contact list page"),
			    QT_TRANSLATE_NOOP("Plugin", "Simple contact list on QML elements"));
		addExtension<QuickPasswordDialog>(
			    QT_TRANSLATE_NOOP("Plugin", "Meego password dialog"),
			    QT_TRANSLATE_NOOP("Plugin", "Simple password dialog on QML elements"));
		addExtension<QuickAuthDialog>(
			    QT_TRANSLATE_NOOP("Plugin", "Meego authorization dialog"),
			    QT_TRANSLATE_NOOP("Plugin", "Simple authorization dialog on QML elements"));
	}
}

QUTIM_EXPORT_PLUGIN(MeegoIntegration::Plugin)

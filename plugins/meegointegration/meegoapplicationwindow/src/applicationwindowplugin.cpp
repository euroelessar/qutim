#include "applicationwindowplugin.h"
#include "applicationwindow.h"
#include "chat.h"
#include "contactlist.h"

namespace MeegoIntegration
{
	void Plugin::init()
	{
		qutim_sdk_0_3::LocalizedString name = QT_TRANSLATE_NOOP("Plugin", "Meego application window");
		qutim_sdk_0_3::LocalizedString description = QT_TRANSLATE_NOOP("Plugin", "Main window of application");
		setInfo(name, description, PLUGIN_VERSION(0, 1, 0, 0));
		addExtension<ApplicationWindow>(name, description);
		addExtension<Chat>(
		            QT_TRANSLATE_NOOP("Plugin", "Meego application window"),
		            QT_TRANSLATE_NOOP("Plugin", "Main window of application"));
		addExtension<ContactList>(
		            QT_TRANSLATE_NOOP("Plugin", "Meego application window"),
		            QT_TRANSLATE_NOOP("Plugin", "Main window of application"));
	}
}

QUTIM_EXPORT_PLUGIN(MeegoIntegration::Plugin)

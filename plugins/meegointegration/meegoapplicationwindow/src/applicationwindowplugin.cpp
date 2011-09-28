#include "applicationwindowplugin.h"
#include "applicationwindow.h"
#include "chat.h"
#include "contactlist.h"

#include "quickpassworddialog.h"
#include "quickauthdialog.h"
#include "quickaddcontactdialog.h"
#include "quickaboutdialog.h"
#include "maemo6idle.h"
#include "quickjoingroupchat.h"

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
		addExtension<Maemo6Idle>(
			    QT_TRANSLATE_NOOP("Plugin", "MeeGo Harmattan Idle detector"),
			    QT_TRANSLATE_NOOP("Plugin", "Simple harmattan idle detector"));
		addExtension<QuickAddContactDialog>(
			    QT_TRANSLATE_NOOP("Plugin", "MeeGo Add contact dialog"),
			    QT_TRANSLATE_NOOP("Plugin", "Simple add contact dialog"));
		addExtension<QuickAboutDialog>(
			    QT_TRANSLATE_NOOP("Plugin", "MeeGo about dialog"),
			    QT_TRANSLATE_NOOP("Plugin", "Simple about dialog"));
		addExtension<QuickJoinGroupChat>(
			    QT_TRANSLATE_NOOP("Plugin", "MeeGo join group chat dialog"),
			    QT_TRANSLATE_NOOP("Plugin", "Simple join group chat dialog"));
	}
}

QUTIM_EXPORT_PLUGIN(MeegoIntegration::Plugin)

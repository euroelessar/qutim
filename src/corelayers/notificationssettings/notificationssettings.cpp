#include "notificationssettings.h"
#include "popupbehavior.h"
#include <libqutim/settingslayer.h>
#include "modulemanagerimpl.h"
#include <libqutim/icon.h>

namespace Core {

	using namespace qutim_sdk_0_3;

	static CoreModuleHelper<NotificationsSettings, StartupModule> emoticons_settings_static(
			QT_TRANSLATE_NOOP("Plugin", "Notifications settings"),
			QT_TRANSLATE_NOOP("Plugin", "Default settings widget set for notifications")
			);

	NotificationsSettings::NotificationsSettings(QObject *parent) :
			QObject(parent)
	{
		GeneralSettingsItem<Core::PopupBehavior> *behavior = new GeneralSettingsItem<Core::PopupBehavior>(Settings::General,Icon("dialog-information"),QT_TRANSLATE_NOOP("Settings","Notifications"));
		Settings::registerItem(behavior);
		deleteLater();
	}

}

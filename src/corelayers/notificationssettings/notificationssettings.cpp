#include "notificationssettings.h"
#include "popupbehavior.h"
#include <qutim/settingslayer.h>
#include <qutim/icon.h>

namespace Core {

	using namespace qutim_sdk_0_3;

	NotificationsSettings::NotificationsSettings(QObject *parent) :
			QObject(parent)
	{
		GeneralSettingsItem<Core::PopupBehavior> *behavior = new GeneralSettingsItem<Core::PopupBehavior>(Settings::General,Icon("dialog-information"),QT_TRANSLATE_NOOP("Settings","Notifications"));
		Settings::registerItem(behavior);
		deleteLater();
	}

}

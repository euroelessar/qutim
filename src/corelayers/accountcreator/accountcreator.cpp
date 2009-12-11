#include "accountcreator.h"
#include "accountcreatorlist.h"
#include "libqutim/icon.h"
#include "src/modulemanagerimpl.h"

namespace Core
{
	static CoreModuleHelper<AccountCreator, StartupModule> acc_creator_static(
			QT_TRANSLATE_NOOP("Plugin", "Account creator"),
			QT_TRANSLATE_NOOP("Plugin", "Default qutIM account creator")
			);

	AccountCreator::AccountCreator()
	{
		SettingsItem *item = new GeneralSettingsItem<AccountCreatorList>(
				Settings::General, Icon("meeting-attending"),
				QT_TRANSLATE_NOOP("Settings", "Accounts"));
		Settings::registerItem(item);
		deleteLater();
	}

	AccountCreator::~AccountCreator()
	{
	}
}

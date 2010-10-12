#include "chatsettings.h"
#include "chatappearance.h"
#include "chatbehavior.h"
#include "../chatlayer/chatlayerimpl.h"
#include <qutim/settingslayer.h>
#include <qutim/icon.h>
#include <qutim/servicemanager.h>

namespace Core
{
namespace AdiumChat
{

ChatSettings::ChatSettings()
{
	QObject *obj = ServiceManager::getByName("ChatForm");
	if (!obj) {
		deleteLater();
		return;
	}
	SettingsItem *item;
	item = new GeneralSettingsItem<ChatAppearance>(Settings::Appearance, Icon("view-choose"),
												   QT_TRANSLATE_NOOP("Settings","Chat"));
	item->connect(SIGNAL(saved()), obj, SLOT(onAppearanceSettingsChanged()));
	Settings::registerItem(item);
	item = new GeneralSettingsItem<ChatBehavior>(Settings::General, Icon("view-choose"),
												 QT_TRANSLATE_NOOP("Settings","Chat"));
	item->connect(SIGNAL(saved()), obj, SLOT(onBehaviorSettingsChanged()));
	Settings::registerItem(item);
	deleteLater();
}

}
}

#include "accountcreator.h"
#include "accountcreatorlist.h"
#include "accountcreatorprotocols.h"
#include "libqutim/icon.h"
#include "src/modulemanagerimpl.h"
#include <libqutim/debug.h>

namespace Core
{
	static CoreModuleHelper<AccountCreator, StartupModule> acc_creator_static(
			QT_TRANSLATE_NOOP("Plugin", "Account creator"),
			QT_TRANSLATE_NOOP("Plugin", "Default qutIM account creator")
			);
	
// 	static CoreModuleHelper<AccountPageCreator> creator_static(
// 			QT_TRANSLATE_NOOP("Plugin", "Account creator (profile mode)"),
// 			QT_TRANSLATE_NOOP("Plugin", "Default qutIM account creator")
// 			);	

	AccountCreator::AccountCreator()
	{
		SettingsItem *item = new GeneralSettingsItem<AccountCreatorList>(
				Settings::General, Icon("meeting-attending"),
				QT_TRANSLATE_NOOP("Settings", "Accounts"));
		item->setPriority(100);
		Settings::registerItem(item);
		deleteLater();

		foreach (Protocol *proto,allProtocols()) {
			if (proto->accounts().count())
				return;
		}

		return; //fixme

		AccountCreatorWizard *wizard = new AccountCreatorWizard();
		wizard->setAttribute(Qt::WA_DeleteOnClose);
#if defined(QUTIM_MOBILE_UI)
		wizard->showMaximized();
#else
		centerizeWidget(wizard);
		wizard->show();
#endif

	}

	AccountCreator::~AccountCreator()
	{
	}
		
//	AccountPageCreator::AccountPageCreator(QObject* parent)
//	{
//		setParent(parent);
//	}
//	QList< QWizardPage* > AccountPageCreator::pages(QWidget* parent)
//	{
//		QList<QWizardPage *> list;
//		list << new AccountCreatorProtocols(static_cast<QWizard*>(parent));
//		return list;
//	}
//	double AccountPageCreator::priority() const
//	{
//		return 50;
//	}

}

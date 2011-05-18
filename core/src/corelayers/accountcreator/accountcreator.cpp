#include "accountcreator.h"
#include "accountcreatorlist.h"
#include "accountcreatorprotocols.h"
#include <qutim/icon.h>
#include <qutim/debug.h>
#include <qutim/systemintegration.h>
#include <QApplication>
#include <QTimer>

namespace Core
{
void AccountCreator::init()
{
	LocalizedString name = QT_TRANSLATE_NOOP("Plugin","Account creator");
	LocalizedString description = QT_TRANSLATE_NOOP("Plugin","Default qutIM account creator");
	setInfo(name, description, QUTIM_VERSION);
	setCapabilities(Loadable);
	// 		addExtension(QT_TRANSLATE_NOOP("Plugin", "Account creator wizard"),
	// 				QT_TRANSLATE_NOOP("Plugin", "Account creator wizard"),
	// 				new GeneralGenerator<AccountPageCreator>()
	// 				);
}

bool AccountCreator::load()
{
	SettingsItem *item = new GeneralSettingsItem<AccountCreatorList>(
				Settings::General, Icon("meeting-attending"),
				QT_TRANSLATE_NOOP("Settings", "Accounts"));
	item->setPriority(100);
	Settings::registerItem(item);

	foreach (Protocol *proto,Protocol::all()) {
		if (!proto->accounts().isEmpty())
			return true;
	}

//#ifdef Q_WS_S60
	QTimer::singleShot(0, this, SLOT(showWizard()));
//#endif
	return true;
}


void AccountCreator::showWizard()
{
	AccountCreatorWizard *wizard = new AccountCreatorWizard();
#ifdef Q_WS_MAEMO5
	wizard->setParent(QApplication::activeWindow());
	wizard->setWindowFlags(wizard->windowFlags() | Qt::Window);
#endif
	wizard->setAttribute(Qt::WA_DeleteOnClose);
	SystemIntegration::show(wizard);
}


bool AccountCreator::unload()
{
	return true;
}

AccountCreator::~AccountCreator()
{
}

AccountPageCreator::AccountPageCreator(QObject* parent)
{
	setParent(parent);
}
QList< QWizardPage* > AccountPageCreator::pages(QWidget* parent)
{
	QList<QWizardPage *> list;
	list << new AccountCreatorProtocols(qobject_cast<QWizard*>(parent));
	return list;
}
double AccountPageCreator::priority() const
{
	return 150;
}

}

QUTIM_EXPORT_PLUGIN(Core::AccountCreator)

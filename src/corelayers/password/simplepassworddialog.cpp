#include "simplepassworddialog.h"
#include "simplepasswordwidget.h"
#include "src/modulemanagerimpl.h"

namespace Core
{
	static CoreModuleHelper<SimplePasswordDialog> acc_creator_static(
			QT_TRANSLATE_NOOP("Plugin", "Simple password dialog"),
			QT_TRANSLATE_NOOP("Plugin", "Default qutIM password dialog")
			);

	SimplePasswordDialog::SimplePasswordDialog()
	{
	}

	void SimplePasswordDialog::setAccount(Account *account)
	{
		SimplePasswordWidget *widget = new SimplePasswordWidget(account);
		connect(this, SIGNAL(destroyed()), widget, SLOT(deleteLater()));
		connect(widget, SIGNAL(rejected()), this, SIGNAL(rejected()));
		connect(widget, SIGNAL(entered(QString,bool)), this, SIGNAL(entered(QString,bool)));
		widget->show();
	}
}

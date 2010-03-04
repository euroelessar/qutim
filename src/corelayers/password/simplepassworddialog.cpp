#include "simplepassworddialog.h"
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
		m_widget = new SimplePasswordWidget(account, this);
		connect(this, SIGNAL(destroyed()), m_widget, SLOT(deleteLater()));
		connect(m_widget, SIGNAL(rejected()), this, SIGNAL(rejected()));
		connect(m_widget, SIGNAL(entered(QString,bool)), this, SIGNAL(entered(QString,bool)));
		m_widget->show();
	}

	void SimplePasswordDialog::setValidator(QValidator *validator)
	{
		if (m_widget)
			m_widget->setValidator(validator);
	}
}

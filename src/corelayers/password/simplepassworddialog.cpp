#include "simplepassworddialog.h"

namespace Core
{
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

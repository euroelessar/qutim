#include "simplepassworddialog.h"

namespace Core
{
SimplePasswordDialog::SimplePasswordDialog()
{
}

void SimplePasswordDialog::setAccount(Account *account)
{
	m_widget = new SimplePasswordWidget(account, this);
	connect(this, SIGNAL(destroyed()), m_widget.data(), SLOT(deleteLater()));
	connect(m_widget.data(), SIGNAL(rejected()), this, SIGNAL(rejected()));
	connect(m_widget.data(), SIGNAL(entered(QString,bool)), this, SIGNAL(entered(QString,bool)));
	m_widget.data()->show();
}

void SimplePasswordDialog::setValidator(QValidator *validator)
{
	if (m_widget)
		m_widget.data()->setValidator(validator);
}
}

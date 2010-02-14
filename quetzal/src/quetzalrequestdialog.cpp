#include "quetzalrequestdialog.h"
#include "quetzalrequest.h"
#include <QLabel>

QuetzalRequestDialog::QuetzalRequestDialog(const char *title, const char *primary,
										   const char *secondary, PurpleRequestType type,
										   void *user_data, QWidget *parent)
				 : QDialog(parent), m_type(type), m_user_data(user_data)
{
	setWindowTitle(title);
	m_boxLayout = new QVBoxLayout(this);
	m_boxLayout->addWidget(new QLabel(quetzal_create_label(primary, secondary), this));
	m_buttonBox = new QDialogButtonBox(Qt::Horizontal, this);
	m_boxLayout->addWidget(m_buttonBox);
//	connect(m_buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(closeRequest()));
	setAttribute(Qt::WA_QuitOnClose, false);
}

void QuetzalRequestDialog::closeRequest()
{
	purple_request_close(m_type, this);
}

void QuetzalRequestDialog::closeEvent(QCloseEvent *e)
{
	closeRequest();
	QDialog::closeEvent(e);
}

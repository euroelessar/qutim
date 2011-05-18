#include "accountcreatorwizard.h"
#include <QAction>

namespace Core
{
AccountCreatorWizard::AccountCreatorWizard()
{
	setAttribute(Qt::WA_DeleteOnClose, true);
	AccountCreatorProtocols *protocolsPage = new AccountCreatorProtocols(this);
	setPage(AccountCreatorProtocols::Id, protocolsPage);
	setWindowTitle(tr("Add a new account"));
	resize(800,600);
	setOption(QWizard::NoBackButtonOnStartPage, true);

//#ifdef QUTIM_MOBILE_UI
//	setOption(QWizard::HaveCustomButton1, true);
//	QPushButton *closeBtn = new QPushButton(tr("Close"), this);
//	connect(closeBtn, SIGNAL(clicked()), SLOT(close()));
//	setButton(QWizard::CustomButton1, closeBtn);
//#endif
}
}

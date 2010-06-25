#include "accountcreatorwizard.h"

namespace Core
{
	AccountCreatorWizard::AccountCreatorWizard()
	{
		setAttribute(Qt::WA_DeleteOnClose, true);
		AccountCreatorProtocols *protocolsPage = new AccountCreatorProtocols(this);
		setPage(AccountCreatorProtocols::Id, protocolsPage);
		setWindowTitle(tr("Add a new account"));
	}
}

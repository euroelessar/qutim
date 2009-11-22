#include "accountcreatorwizard.h"

namespace Core
{
	AccountCreatorWizard::AccountCreatorWizard()
	{
		setAttribute(Qt::WA_DeleteOnClose, true);
		AccountCreatorProtocols *protocolsPage = new AccountCreatorProtocols(this);
		addPage(protocolsPage);
	}
}

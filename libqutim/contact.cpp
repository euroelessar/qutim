#include "contact.h"
#include "account.h"

namespace qutim_sdk_0_3
{
	Contact::Contact(Account *account)
			: QObject(account), m_type(Simple), m_account(account)
	{
	}

	QString Contact::name() const
	{
		return id();
	}
}

#include "jprotocol.h"

namespace Jabber
{
	JProtocol::JProtocol()
	{
	}

	AccountCreationWizard *JProtocol::accountCreationWizard()
	{
		return 0;
	}

	QList<Account *> JProtocol::accounts() const
	{
		return QList<Account *>();
	}

	Account *JProtocol::account(const QString &id) const
	{
		return 0;
	}
}

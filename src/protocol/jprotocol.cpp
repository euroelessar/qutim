#include "jprotocol.h"

namespace Jabber
{
	JProtocol::JProtocol()
	{
	}

	QList<Account *> JProtocol::accounts() const
	{
		return QList<Account *>();
	}

	Account *JProtocol::account(const QString &id) const
	{
		return 0;
	}

	void JProtocol::loadAccounts()
	{
	}
}

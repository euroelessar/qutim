#include "account.h"
#include "contact.h"
#include "protocol.h"

namespace qutim_sdk_0_3
{
	Account::Account(Protocol *protocol) : QObject(protocol)
	{
	}
}

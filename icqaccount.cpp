#include "icqaccount.h"
#include "icqprotocol.h"

IcqAccount::IcqAccount() : Account(IcqProtocol::instance())
{
}

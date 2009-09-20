#include "icqprotocol.h"

IcqProtocol *IcqProtocol::self = 0;

IcqProtocol::IcqProtocol()
{
	Q_ASSERT(!self);
	self = this;
}

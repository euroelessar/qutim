#ifndef ICQPROTOCOL_H
#define ICQPROTOCOL_H

#include <qutim/protocol.h>

using namespace qutim_sdk_0_3;

class IcqProtocol : public Protocol
{
public:
    IcqProtocol();
	static IcqProtocol *instance() { return self; }
private:
	static IcqProtocol *self;
};

#endif // ICQPROTOCOL_H

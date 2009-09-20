#ifndef PROTOCOLNEGOTIATION_H
#define PROTOCOLNEGOTIATION_H

#include "snachandler.h"

class ProtocolNegotiation : public SNACHandler
{
public:
    ProtocolNegotiation();
	virtual void handleSNAC(OscarConnection *conn, const SNAC &snac);
private:
	void setMsgChannelParams(OscarConnection *conn, quint16 chans, quint32 flags);
	quint32 m_login_reqinfo;
};

#endif // PROTOCOLNEGOTIATION_H

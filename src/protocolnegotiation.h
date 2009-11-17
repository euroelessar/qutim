/****************************************************************************
 *  protocolnegotiation.h
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

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

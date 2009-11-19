/****************************************************************************
 *  protocolnegotiation.cpp
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

#include "protocolnegotiation.h"

ProtocolNegotiation::ProtocolNegotiation()
{
	m_infos << SNACInfo(ServiceFamily, ServiceServerReady)
			<< SNACInfo(ServiceFamily, ServiceServerNameInfo)
			<< SNACInfo(ServiceFamily, ServiceServerFamilies2)
			<< SNACInfo(ServiceFamily, ServiceServerRateInfo)
			<< SNACInfo(LocationFamily, LocationRightsReply)
			<< SNACInfo(BuddyFamily, UserSrvReplyBuddy)
			<< SNACInfo(MessageFamily, MessageSrvReplyIcbm)
			<< SNACInfo(BosFamily, PrivacyRightsReply)
			<< SNACInfo(ListsFamily, ListsSrvReplyLists)
			<< SNACInfo(ListsFamily, ListsUpToDate);
	m_login_reqinfo = qrand();
}

void ProtocolNegotiation::handleSNAC(OscarConnection *conn, const SNAC &sn)
{
	switch((sn.family() << 16) | sn.subtype())
	{
	// Server sends supported services list
	case 0x00010003: {
		SNAC snac(ServiceFamily, ServiceClientFamilies);
		// Sending the same as ICQ 6
		snac.appendSimple<quint32>(0x00220001);
		snac.appendSimple<quint32>(0x00010004);
		snac.appendSimple<quint32>(0x00130004);
		snac.appendSimple<quint32>(0x00020001);
		snac.appendSimple<quint32>(0x00030001);
		snac.appendSimple<quint32>(0x00150001);
		snac.appendSimple<quint32>(0x00040001);
		snac.appendSimple<quint32>(0x00060001);
		snac.appendSimple<quint32>(0x00090001);
		snac.appendSimple<quint32>(0x000a0001);
		snac.appendSimple<quint32>(0x000b0001);
		conn->send(snac);
		break; }
	// This is the reply to CLI_REQINFO
	case 0x0001000f: {
		// Skip uin
		Q_UNUSED(sn.readData<quint8>());
		sn.skipData(4);
		// Login
		//qDebug() << (m_login_reqinfo == sn.id());
		if(m_login_reqinfo == sn.id())
		{
			// TLV(x01) User type?
			// TLV(x0C) Empty CLI2CLI Direct connection info
			// TLV(x0A) External IP
			// TLV(x0F) Number of seconds that user has been online
			// TLV(x03) The online since time.
			// TLV(x0A) External IP again
			// TLV(x22) Unknown
			// TLV(x1E) Unknown: empty.
			// TLV(x05) Member of ICQ since.
			// TLV(x14) Unknown
			TLVMap tlvs = sn.readTLVChain();
			quint32 ip = tlvs.value(0x0a).value<quint32>();
			conn->setExternalIP(QHostAddress(ip));
			//qDebug() << conn->externalIP();
		}
		// Else
		else
		{
		}
		break; }
	// Server sends its services version numbers
	case 0x00010018: {
		SNAC snac(ServiceFamily, ServiceClientReqRateInfo);
		conn->send(snac);
		break; }
	// Server sends rate limits information
	case 0x00010007: {

		// Don't know what does it mean
		// TODO: Understand it
		quint16 group_count = sn.readSimple<quint16>();
		SNAC snac(ServiceFamily, ServiceClientRateAck);
		for(int i = 1; i <= group_count; i++)
			snac.appendSimple<quint16>(i);
		conn->send(snac);

		// This command requests from the server certain information about the client that is stored on the server
		// In other words: CLI_REQINFO
		snac.reset(ServiceFamily, ServiceClientReqinfo);
		m_login_reqinfo = conn->send(snac);

		// Request server-stored information (SSI) service limitations
		snac.reset(ListsFamily, ListsCliReqLists);
		snac.appendTLV<quint16>(0x0B, 0x000F); // mimic ICQ 6
		conn->send(snac);

		// Requesting roster
		// TODO: Don't ask full roster each time, see SNAC(13,05) for it
		snac.reset(ListsFamily, ListsCliRequest);
		conn->send(snac);

		// Requesting Location rights
		snac.reset(LocationFamily, LocationCliReqRights);
		conn->send(snac);

		// Requesting client-side contactlist rights
		snac.reset(BuddyFamily, UserCliReqBuddy);
		// Query flags: 1 = Enable Avatars
		//              2 = Enable offline status message notification
		//              4 = Enable Avatars for offline contacts
		//              8 = Use reject for not authorized contacts
		snac.appendTLV<quint16>(0x05, 0x0003); // mimic ICQ 6
		conn->send(snac);

		// Sending CLI_REQICBM
		snac.reset(MessageFamily, MessageCliReqIcbm);
		conn->send(snac);

		// Sending CLI_REQBOS
		snac.reset(BosFamily, PrivacyReqRights);
		conn->send(snac);
		break; }
	// Server replies via location service limitations
	case 0x00020003: {
		// TODO: Implement, it's important
		break; }
	// Server replies via BLM service limitations
	case 0x00030003: {
		break; }
	// Server sends ICBM service parameters to client
	case 0x00040005: {
		quint32 dw_flags = 0x00000303;
		// TODO: Find description
#ifdef DBG_CAPHTML
		dw_flags |= 0x00000400;
#endif
#ifdef DBG_CAPMTN
		dw_flags |= 0x00000008;
#endif
		// Set message parameters for all channels (imitate ICQ 6)
		setMsgChannelParams(conn, 0x0000, dw_flags);
		break; }
	// Server sends PRM service limitations to client
	case 0x00090003: {
		break; }
	// Server sends SSI service limitations to client
	case 0x00130003: {
		// TODO: Implement, it's important
		break; }
	// Server tell client its local copy up-to-date
	case 0x0013000f: {
		break; }
	//
	}
}

void ProtocolNegotiation::setMsgChannelParams(OscarConnection *conn, quint16 chan, quint32 flags)
{
	SNAC snac(MessageFamily, MessageCliSetParams);
	snac.appendSimple<quint16>(chan);                   // Channel
	snac.appendSimple<quint32>(flags);                  // Flags
	snac.appendSimple<quint16>(max_message_snac_size);  // Max message snac size
	snac.appendSimple<quint16>(0x03E7);                 // Max sender warning level
	snac.appendSimple<quint16>(0x03E7);                 // Max receiver warning level
	snac.appendSimple<quint16>(client_rate_limit);      // Minimum message interval in seconds
	snac.appendSimple<quint16>(0x0000);                 // Unknown
	conn->send(snac);
}

/****************************************************************************
 *  messages.cpp
 *
 *  Copyright (c) 2009 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#include "messages.h"
#include "util.h"
#include "buddycaps.h"

namespace Icq {

using namespace Util;

Channel1MessageData::Channel1MessageData(const QString &message, quint16 charset)
{
	QTextCodec *codec = 0;
	if(charset == 0x0002)
		codec = utf16Codec();
	else
		codec = asciiCodec();
	DataUnit msgData;
	msgData.appendSimple(charset);
	msgData.appendData(message, codec);

	appendTLV(0x0501, (quint32)0x0106);
	appendTLV(0x0101, msgData.data());
}

Tlv2711::Tlv2711(quint8 msgType, quint8 msgFlags)
{
	m_cookie = generateCookie();
	appendSimple<quint16>(0x1B, DataUnit::LittleEndian);
	appendSimple(protocol_version);
	appendData(ICQ_CAPABILITY_PSIG_MESSAGE);
	appendSimple(client_features);
	appendSimple(dc_type);
	appendSimple(m_cookie, DataUnit::LittleEndian);
	appendSimple<quint16>(0x0E, DataUnit::LittleEndian);
	appendSimple(m_cookie, DataUnit::LittleEndian);
	appendSimple<quint64>(0); // Unknown 12 bytes
	appendSimple<quint32>(0);
	appendSimple(msgType);
	appendSimple(msgFlags);
}

void Tlv2711::appendXData(quint16 X1, quint16 X2)
{
	appendSimple(X1, LittleEndian);
	appendSimple(X2);
}

void Tlv2711::appendEmptyPacket()
{
	appendSimple<quint16>(1);
	appendSimple<quint8>(0);
}

Channel2BasicMessageData::Channel2BasicMessageData(quint16 command, const Capability &guid, qint64 cookie):
	m_cookie(cookie)
{
	if(m_cookie == 0)
		m_cookie = generateCookie();
	appendSimple(command);
	appendSimple(cookie, DataUnit::LittleEndian);
	appendData(guid);
}

Channel2MessageData::Channel2MessageData(quint16 ackType, const Tlv2711 &data):
	Channel2BasicMessageData(0, ICQ_CAPABILITY_SRVxRELAY, data.cookie())
{
	appendTLV(0x000A, ackType);
	/*if (includeDcInfo)
	{
		/*data.appendTLV(0x03, my_ip);
		data.appendTLV(0x05, my_port);
	}*/
	appendTLV(0x000F);
	appendTLV(0x2711, data.data());
}

ServerMessage::ServerMessage(const QString &uin, const Channel1MessageData &data, bool storeMessage):
	SNAC(MessageFamily, MessageSrvSend)
{
	init(uin, 1);
	appendTLV(0x0002, data.data());
	if(storeMessage)
	{
		// empty TLV(6) store message if recipient offline.
		appendTLV(0x0006);
	}
}

ServerMessage::ServerMessage(const QString &uin, const Channel2BasicMessageData &data):
	SNAC(MessageFamily, MessageSrvSend)
{
	init(uin, 2, data.cookie());
	appendTLV(0x05, data);
}

void ServerMessage::init(const QString &uin, qint16 channel, qint64 cookie)
{
	if(cookie == 0)
		 cookie = generateCookie();
	appendSimple<qint64>(cookie); // cookie
	appendSimple<qint16>(channel); // message channel
	appendData<qint8>(uin); // uid or screenname
}

} // namespace Icq

/****************************************************************************
 *  messages.h
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

#ifndef MESSAGES_H_
#define MESSAGES_H_

#include "snac.h"

namespace Icq {

enum Channel1Codec
{
	CodecUsAscii = 0x0000,
	CodecUtf16Be = 0x0002,
	CodecAnsi    = 0x0003
};

class Channel1MessageData: public DataUnit
{
public:
	Channel1MessageData(const QString &message, Channel1Codec charset = CodecUtf16Be);
};

class Tlv2711: public DataUnit
{
public:
	Tlv2711(quint8 msgType, quint8 msgFlags);
	void appendXData(quint16 X1, quint16 X2);
	void appendEmptyPacket();
	qint64 cookie() const { return m_cookie; };
private:
	qint64 m_cookie;
};

class Channel2BasicMessageData: public DataUnit
{
public:
	Channel2BasicMessageData(quint16 command, const Capability &guid, qint64 cookie = 0);
	qint64 cookie() const { return m_cookie; };
private:
	qint64 m_cookie;
};

class Channel2MessageData: public Channel2BasicMessageData
{
public:
	Channel2MessageData(quint16 ackType, const Tlv2711 &data);
};

class ServerMessage: public SNAC
{
public:
	ServerMessage(const QString &uin, const Channel1MessageData &data, bool storeMessage = true);
	ServerMessage(const QString &uin, const Channel2BasicMessageData &data);
protected:
	void init(const QString &uin, qint16 channel, qint64 cookie = 0);
};

} // namespace Icq

#endif /* MESSAGES_H_ */

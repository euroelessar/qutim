/****************************************************************************
 *  messages.h
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *                        Prokhin Alexey <alexey.prokhin@yandex.ru>
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
#include "snachandler.h"
#include "capability.h"
#include "cookie.h"
#include <QDateTime>

namespace qutim_sdk_0_3 {

namespace oscar {

class IcqAccount;
class IcqContact;

enum Channel1Codec
{
	CodecUsAscii = 0x0000,
	CodecUtf16Be = 0x0002,
	CodecAnsi    = 0x0003
};

enum MessageType
{
	MsgPlain    = 0x01,
	MsgChatReq  = 0x02,
	MsgFileReq  = 0x03,
	MsgUrl      = 0x04,
	MsgAuthReq  = 0x06,
	MsgAuthDeny = 0x07,
	MsgAuthOk   = 0x08,
	MsgServer   = 0x09,
	MsgAdded    = 0x0C,
	MsgWwp      = 0x0D,
	MsgEexpress = 0x0E,
	MsgPlugin   = 0x1A,
	MsgAutoAway = 0xE8,
	MsgAutoBusy = 0xE9,
	MsgAutoNA   = 0xEA,
	MsgAutoDND  = 0xEB,
	MsgAutoFFC  = 0xEC,
	MsgUnknown  = 0x00,
};

// Typing notification statuses
enum MTN
{
	MtnUnknown  = 0xFFFF,
	MtnFinished = 0x0000,
	MtnTyped    = 0x0001,
	MtnBegun    = 0x0002,
	MtnGone     = 0x000F
};

class LIBOSCAR_EXPORT Channel1MessageData: public DataUnit
{
public:
	Channel1MessageData(const QString &message, Channel1Codec charset = CodecUtf16Be);
};

class LIBOSCAR_EXPORT Tlv2711: public DataUnit
{
public:
	Tlv2711(quint8 msgType, quint8 msgFlags, quint16 X1, quint16 X2, const Cookie &cookie = Cookie(true));
	void appendEmptyPacket();
	void appendColors();
	const Cookie &cookie() const { return m_cookie; }
private:
	Cookie m_cookie;
};

class LIBOSCAR_EXPORT Channel2BasicMessageData: public DataUnit
{
public:
	Channel2BasicMessageData(quint16 command, const Capability &guid, const Cookie &cookie = Cookie(true));
	const Cookie &cookie() const { return m_cookie; }
private:
	Cookie m_cookie;
};

class LIBOSCAR_EXPORT Channel2MessageData: public Channel2BasicMessageData
{
public:
	Channel2MessageData(quint16 ackType, const Tlv2711 &data);
};

class LIBOSCAR_EXPORT ServerMessage: public SNAC
{
public:
	ServerMessage();
	ServerMessage(IcqContact *contact, const Channel1MessageData &data, const Cookie &cookie, bool storeMessage = true);
	ServerMessage(IcqContact *contact, const Channel2BasicMessageData &data);
protected:
	void init(IcqContact *contact, qint16 channel, const Cookie &cookie = Cookie(true));
};

class LIBOSCAR_EXPORT ServerResponseMessage: public SNAC
{
public:
	ServerResponseMessage(IcqContact *contact, quint16 format, quint16 reason, const Cookie &cookie = Cookie(true));
};

class MessagePlugin
{
public:
	QSet<Capability> capabilities() { return m_capabilities; }
	virtual void processMessage(IcqContact *contact, const Capability &guid, const QByteArray &data, quint16 type) = 0;
protected:
	QSet<Capability> m_capabilities;
};

typedef QPair<Capability, quint16> Tlv2711Type;
class Tlv2711Plugin
{
public:
	QSet<Tlv2711Type> tlv2711Types() { return m_tlvs2711Types; }
	virtual void processTlvs2711(IcqContact *contact, Capability guid, quint16 type, const DataUnit &data, const Cookie &cookie) = 0;
protected:
	QSet<Tlv2711Type> m_tlvs2711Types;
};

} } // namespace qutim_sdk_0_3::oscar

Q_DECLARE_INTERFACE(qutim_sdk_0_3::oscar::Tlv2711Plugin, "org.qutim.oscar.Tlvs2711Plugin");
Q_DECLARE_INTERFACE(qutim_sdk_0_3::oscar::MessagePlugin, "org.qutim.oscar.MessagePlugin");

#endif /* MESSAGES_H_ */

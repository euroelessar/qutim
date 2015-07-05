/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#ifndef MESSAGES_H_
#define MESSAGES_H_

#include "snac.h"
#include "snachandler.h"
#include "capability.h"
#include "cookie.h"
#include <QDateTime>
#include <QTimer>
#include <qutim/message.h>

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

enum MessageAction
{
	MsgRequest = 0x0000,
	MsgCancel  = 0x0001,
	MsgAccept  = 0x0002
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
	MsgUnknown  = 0x00
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
	Channel1MessageData(const QByteArray &message, Channel1Codec charset = CodecUtf16Be);
	static QByteArray fromUnicode(const QString &message, Channel1Codec charset = CodecUtf16Be);
private:
	void init(const QByteArray &message, Channel1Codec charset);
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

class LIBOSCAR_EXPORT MessagePlugin
{
public:
	virtual ~MessagePlugin();
	QSet<Capability> capabilities() { return m_capabilities; }
	virtual void processMessage(IcqContact *contact, const Capability &guid, const QByteArray &data, quint16 type, quint64 cookie) = 0;
protected:
	QSet<Capability> m_capabilities;
};

typedef QPair<Capability, quint16> Tlv2711Type;
class LIBOSCAR_EXPORT Tlv2711Plugin
{
public:
	virtual ~Tlv2711Plugin();
	QSet<Tlv2711Type> tlv2711Types() { return m_tlvs2711Types; }
	virtual void processTlvs2711(IcqContact *contact, Capability guid, quint16 type, const DataUnit &data, const Cookie &cookie) = 0;
protected:
	QSet<Tlv2711Type> m_tlvs2711Types;
};

} } // namespace qutim_sdk_0_3::oscar

template <>
inline QDebug operator<<(QDebug debug, const qutim_sdk_0_3::oscar::Tlv2711Type &type)
{
	debug << qMakePair(type.first.toString(), type.second);
	return debug;
}

Q_DECLARE_INTERFACE(qutim_sdk_0_3::oscar::Tlv2711Plugin, "org.qutim.oscar.Tlvs2711Plugin");
Q_DECLARE_INTERFACE(qutim_sdk_0_3::oscar::MessagePlugin, "org.qutim.oscar.MessagePlugin");

#endif /* MESSAGES_H_ */


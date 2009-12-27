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
#include "icqcontact_p.h"
#include "util.h"
#include "buddycaps.h"
#include "icqaccount.h"
#include "icqcontact.h"
#include "connection.h"
#include "roster.h"
#include "xtraz.h"
#include <qutim/objectgenerator.h>
#include <qutim/contactlist.h>
#include <qutim/messagesession.h>
#include <qutim/notificationslayer.h>
#include <QHostAddress>

namespace Icq
{

using namespace Util;

Channel1MessageData::Channel1MessageData(const QString &message, Channel1Codec charset)
{
	QTextCodec *codec = 0;
	if (charset == CodecUtf16Be)
		codec = utf16Codec();
	else
		codec = asciiCodec();
	DataUnit msgData;
	msgData.appendSimple<quint16> (charset);
	msgData.appendData(message, codec);

	appendTLV(0x0501, (quint32) 0x0106);
	appendTLV(0x0101, msgData.data());
}

Tlv2711::Tlv2711(quint8 msgType, quint8 msgFlags, quint16 X1, quint16 X2, quint64 cookie)
{
	m_cookie = (cookie == 0) ? generateCookie() : cookie;
	appendSimple<quint16> (0x1B, LittleEndian);
	appendSimple<quint16> (protocol_version, LittleEndian);
	appendData(ICQ_CAPABILITY_PSIG_MESSAGE);
	appendSimple<quint8> (0); // not sure
	appendSimple<quint16> (client_features);
	appendSimple<quint32> (dc_type);
	appendSimple<quint16> (m_cookie, LittleEndian);
	appendSimple<quint16> (0x0E, LittleEndian);
	appendSimple<quint16> (m_cookie, LittleEndian);
	appendSimple<quint64> (0); // Unknown 12 bytes
	appendSimple<quint32> (0);
	appendSimple<quint8> (msgType);
	appendSimple<quint8> (msgFlags);
	appendSimple<quint16> (X1, LittleEndian);
	appendSimple<quint16> (X2, LittleEndian);
}

void Tlv2711::appendEmptyPacket()
{
	appendSimple<quint16> (1, LittleEndian);
	appendSimple<quint8> (0);
}

void Tlv2711::appendColors()
{
	appendSimple<quint32> (0x00000000); // foreground
	appendSimple<quint32> (0x00FFFFFF, LittleEndian); // background
}

Channel2BasicMessageData::Channel2BasicMessageData(quint16 command, const Capability &guid, qint64 cookie) :
	m_cookie(cookie)
{
	if (m_cookie == 0)
		m_cookie = generateCookie();
	appendSimple(command);
	appendSimple(cookie, LittleEndian);
	appendData(guid);
}

Channel2MessageData::Channel2MessageData(quint16 ackType, const Tlv2711 &data) :
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

ServerMessage::ServerMessage() :
	SNAC(MessageFamily, MessageSrvSend)
{

}

ServerMessage::ServerMessage(const QString &uin, const Channel1MessageData &data, bool storeMessage) :
	SNAC(MessageFamily, MessageSrvSend)
{
	init(uin, 1);
	appendTLV(0x0002, data.data());
	if (storeMessage) {
		// empty TLV(6) store message if recipient offline.
		appendTLV(0x0006);
	}
}

ServerMessage::ServerMessage(const QString &uin, const Channel2BasicMessageData &data) :
	SNAC(MessageFamily, MessageSrvSend)
{
	init(uin, 2, data.cookie());
	appendTLV(0x05, data.data());
}

void ServerMessage::init(const QString &uin, qint16 channel, qint64 cookie)
{
	if (cookie == 0)
		cookie = generateCookie();
	appendSimple<qint64> (cookie); // cookie
	appendSimple<qint16> (channel); // message channel
	appendData<qint8> (uin); // uid or screenname
}

ServerResponseMessage::ServerResponseMessage(const QString &uin, quint16 format, quint16 reason, quint64 cookie) :
	SNAC(MessageFamily, MessageResponse)
{
	if (cookie == 0)
		cookie = generateCookie();
	appendSimple<quint64> (cookie);
	appendSimple<quint16> (format);
	appendData<quint8> (uin);
	appendSimple<quint16> (reason);
}

MessagesHandler::MessagesHandler(IcqAccount *account, QObject *parent) :
	SNACHandler(parent), m_account(account)
{
	m_infos << SNACInfo(MessageFamily, MessageSrvReplyIcbm)
			<< SNACInfo(MessageFamily, MessageResponse)
			<< SNACInfo(MessageFamily, MessageSrvRecv)
			<< SNACInfo(MessageFamily, MessageSrvAck)
			<< SNACInfo(MessageFamily, MessageMtn)
			<< SNACInfo(MessageFamily, MessageSrvError);
	foreach(const ObjectGenerator *gen, moduleGenerators<MessagePlugin>())
	{
		MessagePlugin *plugin = gen->generate<MessagePlugin> ();
		foreach(const Capability &cap, plugin->capabilities())
			m_msg_plugins.insert(cap, plugin);
	}
}

void MessagesHandler::handleSNAC(AbstractConnection *conn, const SNAC &sn)
{
	Q_ASSERT((AbstractConnection*)m_account->connection() == conn);
	switch ((sn.family() << 16) | sn.subtype()) {
	case MessageFamily << 16 | MessageSrvReplyIcbm:
		debug() << IMPLEMENT_ME << "MessageFamily, MessageSrvReplyIcbm";
		break;
	case MessageFamily << 16 | MessageSrvRecv:
		handleMessage(sn);
		break;
	case MessageFamily << 16 | MessageResponse:
		handleResponse(sn);
		break;
	case MessageFamily << 16 | MessageSrvAck: {
		sn.skipData(8); // skip cookie.
		quint16 channel = sn.readSimple<quint16> ();
		QString uin = sn.readString<qint8> ();
		debug() << QString("Server accepted message for delivery to %1 on channel %2"). arg(uin).arg(channel);
		break;
	}
	// Typing notifications
	case MessageFamily << 16 | MessageMtn: {
		sn.skipData(8); // skip cookie.
		quint16 channel = sn.readSimple<quint16> ();
		QString uin = sn.readString<qint8> ();
		quint16 type = sn.readSimple<quint16> ();
		IcqContact *contact = m_account->roster()->contact(uin);
		if (contact) {
			ChatState newState;
			if (/*type == 0 || */type == 1)
				newState = ChatStatePaused;
			else if (type == 2)
				newState = ChatStateComposing;
			debug() << contact->id() << "typing state changed to" << type;
			emit contact->chatStateChanged(newState);
		}
		break;
	}
	case MessageFamily << 16 | MessageSrvError: {
		ProtocolError error(sn);
		debug() << QString("Error (%1, %2): %3"). arg(error.code, 2, 16).arg(error.subcode, 2, 16).arg(error.str);
		break;
	}
	}
}

void MessagesHandler::handleMessage(const SNAC &snac)
{
	quint64 cookie = snac.readSimple<quint64> ();
	quint16 channel = snac.readSimple<quint16> ();
	QString uin = snac.readString<quint8> ();
	IcqContact *contact = m_account->roster()->contact(uin);
	quint16 warning = snac.readSimple<quint16> ();
	snac.skipData(2); // unused number of tlvs
	TLVMap tlvs = snac.readTLVChain();
	QString message;
	QDateTime time;
	switch (channel) {
	case 0x0001: // message
		handleChannel1Message(snac, contact, uin, tlvs);
		break;
	case 0x0002: // rendezvous
		handleChannel2Message(snac, contact, uin, tlvs, cookie);
		break;
	case 0x0004:
		handleChannel4Message(snac, contact, uin, tlvs);
		break;
	default:
		qWarning("Unknown message channel: %d", int(channel));
	}
}

void MessagesHandler::handleResponse(const SNAC &snac)
{
	quint64 cookie = snac.readSimple<quint64> ();
	quint16 format = snac.readSimple<quint16> ();
	if (format != 2) {
		debug() << "Unknown response format" << format;
		return;
	}
	QString uin = snac.readString<quint8> ();
	IcqContact *contact = m_account->roster()->contact(uin);
	if (!contact) {
		debug() << "Response message from unknown contact" << uin;
		return;
	}
	//quint16 reason = snac.readSimple<quint16>();
	snac.skipData(2);
	handleTlv2711(snac, contact, 2, cookie);
}

void MessagesHandler::handleChannel1Message(const SNAC &snac, IcqContact *contact, const QString &uin, const TLVMap &tlvs)
{
	Q_UNUSED(contact);
	QString message;
	QDateTime time;
	if (tlvs.contains(0x0002)) {
		DataUnit data(tlvs.value(0x0002));
		TLVMap msg_tlvs = data.readTLVChain();
		if (msg_tlvs.contains(0x0501))
			debug(Verbose) << "Message has" << msg_tlvs.value(0x0501).value().toHex().constData() << "caps";
		foreach(const TLV &tlv, msg_tlvs.values(0x0101))
		{
			DataUnit msg_data(tlv);
			quint16 charset = msg_data.readSimple<quint16> ();
			quint16 codepage = msg_data.readSimple<quint16> ();
			QByteArray data = msg_data.readAll();
			QTextCodec *codec = 0;
			if (charset == CodecUtf16Be)
				codec = utf16Codec();
			else
				codec = asciiCodec();
			message += codec->toUnicode(data);
		}
		if (!(snac.id() & 0x80000000) && msg_tlvs.contains(0x0016)) // Offline message
			time = QDateTime::fromTime_t(msg_tlvs.value(0x0016).value<quint32> ());
		appendMessage(contact, message, time);
	} else {
		debug() << "Incorrect message on channel 1 from" << uin << ": SNAC should contain TLV 2";
	}
}

void MessagesHandler::handleChannel2Message(const SNAC &snac, IcqContact *contact, const QString &uin, const TLVMap &tlvs, quint64 msgCookie)
{
	if (tlvs.contains(0x0005)) {
		DataUnit data(tlvs.value(0x0005));
		quint16 type = data.readSimple<quint16> ();
		data.skipData(8); // again cookie
		Capability guid = data.readCapability();
		if (guid.isEmpty()) {
			debug() << "Incorrect message on channel 2 from" << uin << ": guid is not found";
			return;
		}
		if (guid == ICQ_CAPABILITY_SRVxRELAY) {
			if (type == 1) {
				debug() << "Abort messages on channel 2 is ignored";
				return;
			}
			TLVMap tlvs = data.readTLVChain();
			quint16 ack = tlvs.value(0x0A).value<quint16> ();
			if (contact) {
				if (tlvs.contains(0x03))
					contact->d_func()->dc_info.external_ip = QHostAddress(tlvs.value(0x04).value<quint32> ());
				if (tlvs.contains(0x04))
					contact->d_func()->dc_info.internal_ip = QHostAddress(tlvs.value(0x04).value<quint32> ());
				if (tlvs.contains(0x04))
					contact->d_func()->dc_info.port = tlvs.value(0x05).value<quint32> ();
			}
			if (tlvs.contains(0x2711)) {
				DataUnit data(tlvs.value(0x2711));
				handleTlv2711(data, contact, ack, msgCookie);

			} else
				debug() << "Message on channel 2 should contain TLV 2711";
		} else {
			QList<MessagePlugin *> plugins = m_msg_plugins.values(guid);
			if (!plugins.isEmpty()) {
				QByteArray plugin_data = data.readAll();
				for (int i = 0; i < plugins.size(); i++)
					plugins.at(i)->processMessage(uin, guid, plugin_data, type);
			} else
				debug() << IMPLEMENT_ME << QString("Message (channel 2) from %1 with type %2 and guid %3 is not processed."). arg(uin).arg(type).arg(guid.toString());
		}
	} else
		debug() << "Incorrect message on channel 2 from" << uin << ": SNAC should contain TLV 5";
}

void MessagesHandler::handleChannel4Message(const SNAC &snac, IcqContact *contact, const QString &uin, const TLVMap &tlvs)
{
	// TODO: Understand this holy shit
	if (tlvs.contains(0x0005)) {
		DataUnit data(tlvs.value(0x0005));
		quint32 uin_2 = data.readSimple<quint32> (LittleEndian);
		if (QString::number(uin_2) != uin)
			return;
		quint8 type = data.readSimple<quint8> ();
		quint8 flags = data.readSimple<quint8> ();
		QByteArray msg_data = data.readData<quint16> (LittleEndian);
		debug() << IMPLEMENT_ME << QString("Message (channel 3) from %1 with type %2 is not processed."). arg(uin).arg(type);
	} else
		debug() << "Incorrect message on channel 4 from" << uin << ": SNAC should contain TLV 5";
}

void MessagesHandler::handleTlv2711(const DataUnit &data, IcqContact *contact, quint16 ack, quint64 msgCookie)
{
	quint16 id = data.readSimple<quint16> (LittleEndian);
	if (id != 0x1B) {
		debug() << "Unknown message id in TLV 2711";
		return;
	}
	quint16 version = data.readSimple<quint16> (LittleEndian);
	if (contact)
		contact->d_func()->version = version;
	Capability guid = data.readCapability();
	data.skipData(9);
	id = data.readSimple<quint16> (LittleEndian);
	quint16 cookie = data.readSimple<quint16> (LittleEndian);
	if (guid == ICQ_CAPABILITY_PSIG_MESSAGE) {
		data.skipData(12);
		quint8 type = data.readSimple<quint8> ();
		quint8 flags = data.readSimple<quint8> ();
		quint16 status = data.readSimple<quint16> (LittleEndian);
		quint16 priority = data.readSimple<quint16> (LittleEndian);

		if (type == MsgPlain && ack != 2) // Plain message
		{
			sendChannel2Response(contact, type, flags, msgCookie);
			QByteArray message_data = data.readData<quint16> (LittleEndian);
			message_data.resize(message_data.size() - 1);
			QColor foreground(data.readSimple<quint8> (), data.readSimple<quint8> (), data.readSimple<quint8> (), data.readSimple<quint8> ());
			QColor background(data.readSimple<quint8> (), data.readSimple<quint8> (), data.readSimple<quint8> (), data.readSimple<quint8> ());
			QTextCodec *codec = NULL;
			while (data.dataSize() > 0) {
				QString guid = data.readString<quint32> (LittleEndian);
				if (guid.compare(ICQ_CAPABILITY_UTF8.toString(), Qt::CaseInsensitive) == 0) {
					codec = utf8Codec();
				}
				if (guid.compare(ICQ_CAPABILITY_RTFxMSGS.toString(), Qt::CaseInsensitive) == 0) {
					debug() << "RTF is not supported";
					return;
				}
			}
			if (codec == NULL)
				codec = asciiCodec();
			appendMessage(contact, codec->toUnicode(message_data));
		} else if (MsgPlugin) {
			data.readData<quint16> (LittleEndian);
			DataUnit info = data.readData<quint16> (LittleEndian);
			Capability pluginType = info.readCapability().data();
			quint16 pluginId = info.readSimple<quint16> (LittleEndian);
			QString pluginName = info.readString<quint32> (LittleEndian);
			DataUnit pluginData = data.readData<quint32> (LittleEndian);
			if (pluginType == MSG_XSTRAZ_SCRIPT) {
				Xtraz::handleXtraz(contact, pluginId, pluginData, msgCookie);
			} else
				debug() << "Unhandled plugin message" << pluginType.toString() << pluginId << pluginName << pluginData.data().toHex();
		} else
			debug() << "Unhandled TLV 2711 message with type" << hex << type;
	} else
		debug() << "Unknown format of TLV 2711";
}

void MessagesHandler::appendMessage(IcqContact *contact, const QString &message, QDateTime time)
{
	if (!time.isValid())
		time = QDateTime::currentDateTime();
	debug() << "Received message" << contact->name() << time << message;
	if (ChatLayer::instance()) {
		ChatSession *session = ChatLayer::instance()->getSession(m_account, contact);
		Message m;
		m.setIncoming(true);
		if (contact->HtmlSupport())
			m.setProperty("html", message);
		m.setText(message);
		m.setTime(time);
		m.setChatUnit(session->getUnit());
		session->appendMessage(m);
	}
}

void MessagesHandler::sendChannel2Response(IcqContact *contact, quint8 type, quint8 flags, quint64 cookie)
{
	Tlv2711 responseTlv(type, flags, 0, 0);
	responseTlv.appendEmptyPacket();
	responseTlv.appendColors();
	ServerResponseMessage response(contact->id(), 2, 3, cookie);
	response.appendData(responseTlv.data());
	m_account->connection()->send(response);
}

} // namespace Icq

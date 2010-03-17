/****************************************************************************
 *  messages.cpp
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

#include "messages_p.h"
#include "icqcontact_p.h"
#include "icqaccount_p.h"
#include "icqprotocol.h"
#include "util.h"
#include "buddycaps.h"
#include "connection.h"
#include <qutim/objectgenerator.h>
#include <qutim/contactlist.h>
#include <qutim/notificationslayer.h>
#include <QHostAddress>
#include <QApplication>

namespace qutim_sdk_0_3 {

namespace oscar {

using namespace Util;

Channel1MessageData::Channel1MessageData(const QString &message, Channel1Codec charset)
{
	QTextCodec *codec = 0;
	if (charset == CodecUtf16Be)
		codec = utf16Codec();
	else
		codec = asciiCodec();
	DataUnit msgData;
	msgData.append<quint16>(charset);
	msgData.append(message, codec);

	appendTLV(0x0501, (quint32) 0x0106);
	appendTLV(0x0101, msgData.data());
}

Tlv2711::Tlv2711(quint8 msgType, quint8 msgFlags, quint16 X1, quint16 X2, const Cookie &cookie):
	m_cookie(cookie)
{
	Q_ASSERT(!cookie.isEmpty());
	append<quint16>(0x1B, LittleEndian);
	append<quint16>(protocol_version, LittleEndian);
	append(ICQ_CAPABILITY_PSIG_MESSAGE);
	append<quint8>(0); // not sure
	append<quint16>(client_features);
	append<quint32>(dc_type);
	append<quint16>(m_cookie.id(), LittleEndian);
	append<quint16>(0x0E, LittleEndian);
	append<quint16>(m_cookie.id(), LittleEndian);
	append<quint64>(0); // Unknown 12 bytes
	append<quint32>(0);
	append<quint8>(msgType);
	append<quint8>(msgFlags);
	append<quint16>(X1, LittleEndian);
	append<quint16>(X2, LittleEndian);
}

void Tlv2711::appendEmptyPacket()
{
	append<quint16>(1, LittleEndian);
	append<quint8>(0);
}

void Tlv2711::appendColors()
{
	append<quint32>(0x00000000); // foreground
	append<quint32>(0x00FFFFFF, LittleEndian); // background
}

Channel2BasicMessageData::Channel2BasicMessageData(quint16 command, const Capability &guid, const Cookie &cookie):
	m_cookie(cookie)
{
	Q_ASSERT(!cookie.isEmpty());
	append(command);
	append(m_cookie);
	append(guid);
}

Channel2MessageData::Channel2MessageData(quint16 ackType, const Tlv2711 &data) :
	Channel2BasicMessageData(0, ICQ_CAPABILITY_SRVxRELAY, data.cookie())
{
	appendTLV(0x000A, ackType);
	/*if (includeDcInfo)
	{
		data.appendTLV(0x03, my_ip);
		data.appendTLV(0x05, my_port);
	}*/
	appendTLV(0x000F);
	appendTLV(0x2711, data.data());
}

ServerMessage::ServerMessage() :
	SNAC(MessageFamily, MessageSrvSend)
{

}

ServerMessage::ServerMessage(IcqContact *contact, const Channel1MessageData &data, const Cookie &cookie, bool storeMessage) :
	SNAC(MessageFamily, MessageSrvSend)
{
	init(contact, 1, cookie);
	appendTLV(0x0002, data.data());
	if (storeMessage) {
		// empty TLV(6) store message if recipient offline.
		appendTLV(0x0006);
	}
}

ServerMessage::ServerMessage(IcqContact *contact, const Channel2BasicMessageData &data) :
	SNAC(MessageFamily, MessageSrvSend)
{
	init(contact, 2, data.cookie());
	appendTLV(0x05, data.data());
}

void ServerMessage::init(IcqContact *contact, qint16 channel, const Cookie &cookie)
{
	Q_ASSERT(!cookie.isEmpty());
	append(cookie); // cookie
	append<quint16>(channel); // message channel
	append<quint8>(contact->id()); // uid or screenname
}

ServerResponseMessage::ServerResponseMessage(IcqContact *contact, quint16 format, quint16 reason, const Cookie &cookie) :
	SNAC(MessageFamily, MessageResponse)
{
	Q_ASSERT(!cookie.isEmpty());
	append(cookie);
	append<quint16>(format);
	append<quint8>(contact->id());
	append<quint16>(reason);
}

MessagesHandler::MessagesHandler(IcqAccount *account, QObject *parent) :
	QObject(parent), m_account(account)
{
	connect(account, SIGNAL(loginFinished()), SLOT(loginFinished()));
	connect(account, SIGNAL(settingsUpdated()), SLOT(settingsUpdated()));
	m_infos << SNACInfo(ServiceFamily, ServiceServerAsksServices)
			<< SNACInfo(MessageFamily, MessageSrvReplyIcbm)
			<< SNACInfo(MessageFamily, MessageResponse)
			<< SNACInfo(MessageFamily, MessageSrvRecv)
			<< SNACInfo(MessageFamily, MessageSrvAck)
			<< SNACInfo(MessageFamily, MessageMtn)
			<< SNACInfo(MessageFamily, MessageSrvError)
			<< SNACInfo(ExtensionsFamily, ExtensionsMetaSrvReply);
	foreach(const ObjectGenerator *gen, moduleGenerators<MessagePlugin>())
	{
		MessagePlugin *plugin = gen->generate<MessagePlugin>();
		foreach(const Capability &cap, plugin->capabilities())
			m_msg_plugins.insert(cap, plugin);
	}

	foreach(const ObjectGenerator *gen, moduleGenerators<Tlv2711Plugin>()) {
		Tlv2711Plugin *plugin = gen->generate<Tlv2711Plugin>();
		Q_ASSERT(plugin);
		foreach (Tlv2711Type type, plugin->tlv2711Types())
			m_tlvs2711Plugins.insert(type, plugin);
	}
}

MessagesHandler::~MessagesHandler()
{
}

void MessagesHandler::registerMessagePlugin(MessagePlugin *plugin)
{
	foreach(const Capability &cap, plugin->capabilities())
		m_msg_plugins.insert(cap, plugin);
}

void MessagesHandler::registerTlv2711Plugin(Tlv2711Plugin *plugin)
{
	foreach (Tlv2711Type type, plugin->tlv2711Types())
		m_tlvs2711Plugins.insert(type, plugin);
}

void MessagesHandler::handleSNAC(AbstractConnection *conn, const SNAC &sn)
{
	Q_ASSERT((AbstractConnection*)m_account->connection() == conn);
	switch ((sn.family() << 16) | sn.subtype()) {
	case ServiceFamily << 16 | ServiceServerAsksServices: {
		// Sending CLI_REQICBM
		SNAC snac(MessageFamily, MessageCliReqIcbm);
		conn->send(snac);
		break;
	}
	// Server sends ICBM service parameters to client
	case MessageFamily << 16 | MessageSrvReplyIcbm: {
		debug() << IMPLEMENT_ME << "MessageFamily, MessageSrvReplyIcbm";
		quint32 dw_flags = 0x00000303;
		// TODO: Find description
#ifdef DBG_CAPHTML
		dw_flags |= 0x00000400;
#endif
		dw_flags |= 0x00000008; // typing notifications
		// Set message parameters for all channels (imitate ICQ 6)
		SNAC snac(MessageFamily, MessageCliSetParams);
		snac.append<quint16>(0x0000); // Channel
		snac.append<quint32>(dw_flags); // Flags
		snac.append<quint16>(max_message_snac_size); // Max message snac size
		snac.append<quint16>(0x03E7); // Max sender warning level
		snac.append<quint16>(0x03E7); // Max receiver warning level
		snac.append<quint16>(client_rate_limit); // Minimum message interval in seconds
		snac.append<quint16>(0x0000); // Unknown
		conn->send(snac);
		break;
	}
	case MessageFamily << 16 | MessageSrvRecv:
		handleMessage(sn);
		break;
	case MessageFamily << 16 | MessageResponse:
		handleResponse(sn);
		break;
	case MessageFamily << 16 | MessageSrvAck: {
		sn.skipData(8); // skip cookie.
		quint16 channel = sn.read<quint16>();
		QString uin = sn.read<QString, qint8>();
		debug() << QString("Server accepted message for delivery to %1 on channel %2"). arg(uin).arg(channel);
		break;
	}
	// Typing notifications
	case MessageFamily << 16 | MessageMtn: {
		sn.skipData(8); // skip cookie.
		quint16 channel = sn.read<quint16>();
		Q_UNUSED(channel);
		QString uin = sn.read<QString, qint8>();
		quint16 type = sn.read<quint16>();
		IcqContact *contact = m_account->getContact(uin);
		if (contact) {
			ChatState newState;
			if (type == MtnFinished) {
				ChatState old = contact->chatState();
				if (old == ChatStateActive || old == ChatStateInActive)
					newState = ChatStateInActive;
				else
					newState = ChatStateActive;
			} else if (type == MtnTyped) {
				newState = ChatStatePaused;
			} else if (type == MtnBegun) {
				newState = ChatStateComposing;
			} else if (type == MtnGone) {
				newState = ChatStateGone;
			} else {
				debug() << "Unknown typing notification from"
						<< contact->id() << ", type" << type;
			}
			debug() << contact->name() << "typing state changed to" << type;
			contact->setChatState(newState);
		}
		break;
	}
	case ExtensionsFamily << 16 | ExtensionsMetaSrvReply: {
		TLVMap tlvs = sn.read<TLVMap>();
		if (tlvs.contains(0x01)) {
			DataUnit data(tlvs.value(0x01));
			data.skipData(6); // skip length field + my uin
			quint16 metaType = data.read<quint16>(LittleEndian);
			switch (metaType) {
			case (0x0041):
				// Offline message.
				// It seems it's not used anymore.
				break;
			case (0x0042):
				// Delete offline messages from the server.
				sendMetaInfoRequest(0x003E);
				break;
			}
		}
		break;
	}
	case MessageFamily << 16 | MessageSrvError: {
		ProtocolError error(sn);
		debug() << QString("Error (%1, %2): %3")
				.arg(error.code(), 2, 16)
				.arg(error.subcode(), 2, 16)
				.arg(error.errorString());
		break;
	}
	}
}

void MessagesHandler::loginFinished()
{
	// Offline messages request
	sendMetaInfoRequest(0x003C);
}

void MessagesHandler::settingsUpdated()
{
	m_detectCodec = m_account->protocol()->config("general").value("detectCodec", true);
}

void MessagesHandler::handleMessage(const SNAC &snac)
{
	quint64 cookie = snac.read<quint64>();
	quint16 channel = snac.read<quint16>();
	QString uin = snac.read<QString, quint8>();
	IcqContact *contact = m_account->getContact(uin, true);
	quint16 warning = snac.read<quint16>();
	Q_UNUSED(warning);
	snac.skipData(2); // unused number of tlvs
	TLVMap tlvs = snac.read<TLVMap>();

	QString message;
	switch (channel) {
	case 0x0001: // message
		message = handleChannel1Message(snac, contact, tlvs);
		break;
	case 0x0002: // rendezvous
		message = handleChannel2Message(snac, contact, tlvs, cookie);
		break;
	case 0x0004:
		message = handleChannel4Message(snac, contact, tlvs);
		break;
	default:
		qWarning("Unknown message channel: %d", int(channel));
	}
	if (!message.isEmpty()) {
		Message m;
		if (tlvs.contains(0x0016))
			m.setTime(QDateTime::fromTime_t(tlvs.value(0x0016).read<quint32>()));
		else
			m.setTime(QDateTime::currentDateTime());
		m.setIncoming(true);
		ChatSession *session = ChatLayer::instance()->getSession(m_account, contact);
		m.setChatUnit(session->getUnit());
		m.setText(message);
		session->appendMessage(m);
	}
}

void MessagesHandler::handleResponse(const SNAC &snac)
{
	Cookie cookie = snac.read<Cookie>();
	quint16 format = snac.read<quint16>();
	if (format != 2) {
		debug() << "Unknown response format" << format;
		return;
	}
	QString uin = snac.read<QString, quint8>();
	IcqContact *contact = m_account->getContact(uin);
	cookie.setContact(contact);
	if (!contact) {
		debug() << "Response message from unknown contact" << uin;
		return;
	}
	//quint16 reason = snac.read<quint16>();
	snac.skipData(2);
	handleTlv2711(snac, contact, 2, cookie);
}

QString MessagesHandler::handleChannel1Message(const SNAC &snac, IcqContact *contact, const TLVMap &tlvs)
{
	Q_UNUSED(contact);
	QString message;
	if (tlvs.contains(0x0002)) {
		DataUnit data(tlvs.value(0x0002));
		TLVMap msg_tlvs = data.read<TLVMap>();
		if (msg_tlvs.contains(0x0501))
			debug(Verbose) << "Message has" << msg_tlvs.value(0x0501).data().toHex().constData() << "caps";
		foreach(const TLV &tlv, msg_tlvs.values(0x0101))
		{
			DataUnit msg_data(tlv);
			quint16 charset = msg_data.read<quint16>();
			quint16 codepage = msg_data.read<quint16>();
			Q_UNUSED(codepage);
			QByteArray data = msg_data.readAll();
			QTextCodec *codec = 0;
			if (charset == CodecUtf16Be)
				codec = utf16Codec();
			else if (m_detectCodec)
				codec = detectCodec();
			else
				codec = asciiCodec();
			message += codec->toUnicode(data);
		}
	} else {
		debug() << "Incorrect message on channel 1 from" << contact->id() << ": SNAC should contain TLV 2";
	}
	debug() << "New message has been received on channel 1:" << message;
	return message;
}

QString MessagesHandler::handleChannel2Message(const SNAC &snac, IcqContact *contact, const TLVMap &tlvs, quint64 msgCookie)
{
	QString uin = contact->id();
	if (tlvs.contains(0x0005)) {
		DataUnit data(tlvs.value(0x0005));
		quint16 type = data.read<quint16>();
		data.skipData(8); // again cookie
		Capability guid = data.read<Capability>();
		if (guid.isEmpty()) {
			debug() << "Incorrect message on channel 2 from" << uin << ": guid is not found";
			return QString();
		}
		if (guid == ICQ_CAPABILITY_SRVxRELAY) {
			if (type == 1) {
				debug() << "Abort messages on channel 2 is ignored";
				return QString();
			}
			TLVMap tlvs = data.read<TLVMap>();
			quint16 ack = tlvs.value(0x0A).read<quint16>();
			if (contact) {
				if (tlvs.contains(0x03))
					contact->d_func()->dc_info.external_ip = QHostAddress(tlvs.value(0x04).read<quint32>());
				if (tlvs.contains(0x04))
					contact->d_func()->dc_info.internal_ip = QHostAddress(tlvs.value(0x04).read<quint32>());
				if (tlvs.contains(0x04))
					contact->d_func()->dc_info.port = tlvs.value(0x05).read<quint32>();
			}
			if (tlvs.contains(0x2711)) {
				DataUnit data(tlvs.value(0x2711));
				return handleTlv2711(data, contact, ack, msgCookie);
			} else
				debug() << "Message on channel 2 should contain TLV 2711";
		} else {
			QList<MessagePlugin *> plugins = m_msg_plugins.values(guid);
			if (!plugins.isEmpty()) {
				QByteArray plugin_data = data.readAll();
				for (int i = 0; i < plugins.size(); i++)
					plugins.at(i)->processMessage(contact, guid, plugin_data, type, msgCookie);
			} else {
				debug() << IMPLEMENT_ME
						<< QString("Message (channel 2) from %1 with type %2 and guid %3 is not processed.")
						.arg(uin)
						.arg(type)
						.arg(guid.toString());
			}
		}
	} else
		debug() << "Incorrect message on channel 2 from" << uin << ": SNAC should contain TLV 5";
	return QString();
}

QString MessagesHandler::handleChannel4Message(const SNAC &snac, IcqContact *contact, const TLVMap &tlvs)
{
	Q_UNUSED(snac);
	QString uin = contact->id();
	// TODO: Understand this holy shit
	if (tlvs.contains(0x0005)) {
		DataUnit data(tlvs.value(0x0005));
		quint32 uin_2 = data.read<quint32>(LittleEndian);
		if (QString::number(uin_2) != uin)
			return QString();
		quint8 type = data.read<quint8>();
		quint8 flags = data.read<quint8>();
		QByteArray msg_data = data.read<QByteArray, quint16>(LittleEndian);
		Q_UNUSED(flags);
		Q_UNUSED(msg_data);
		debug() << IMPLEMENT_ME << QString("Message (channel 3) from %1 with type %2 is not processed."). arg(uin).arg(type);
	} else
		debug() << "Incorrect message on channel 4 from" << uin << ": SNAC should contain TLV 5";
	return QString();
}

QString MessagesHandler::handleTlv2711(const DataUnit &data, IcqContact *contact, quint16 ack, const Cookie &msgCookie)
{
	if (ack == 2 && !msgCookie.unlock()) {
		debug().nospace() << "Unexpected response message is skiped. Cookie:" << msgCookie.id();
		return QString();
	}
	quint16 id = data.read<quint16>(LittleEndian);
	if (id != 0x1B) {
		debug() << "Unknown message id in TLV 2711";
		return QString();
	}
	quint16 version = data.read<quint16>(LittleEndian);
	if (contact)
		contact->d_func()->version = version;
	Capability guid = data.read<Capability>();
	data.skipData(9);
	id = data.read<quint16>(LittleEndian);
	quint16 cookie = data.read<quint16>(LittleEndian);
	Q_UNUSED(cookie);
	if (guid == ICQ_CAPABILITY_PSIG_MESSAGE) {
		data.skipData(12);
		quint8 type = data.read<quint8>();
		quint8 flags = data.read<quint8>();
		quint16 status = data.read<quint16>(LittleEndian);
		quint16 priority = data.read<quint16>(LittleEndian);
		Q_UNUSED(status);
		Q_UNUSED(priority);

		if (type == MsgPlain && ack != 2) // Plain message
		{
			sendChannel2Response(contact, type, flags, msgCookie);
			QByteArray message_data = data.read<QByteArray, quint16>(LittleEndian);
			message_data.resize(message_data.size() - 1);
			QColor foreground(data.read<quint8>(),
							  data.read<quint8>(),
							  data.read<quint8>(),
							  data.read<quint8>());
			QColor background(data.read<quint8>(),
							  data.read<quint8>(),
							  data.read<quint8>(),
							  data.read<quint8>());
			Q_UNUSED(foreground);
			Q_UNUSED(background);
			QTextCodec *codec = NULL;
			while (data.dataSize() > 0) {
				QString guid = data.read<QString, quint32>(LittleEndian);
				if (!m_detectCodec) {
					if (guid.compare(ICQ_CAPABILITY_UTF8.toString(), Qt::CaseInsensitive) == 0) {
						codec = utf8Codec();
					}
				}
				if (guid.compare(ICQ_CAPABILITY_RTFxMSGS.toString(), Qt::CaseInsensitive) == 0) {
					debug() << "RTF is not supported";
					return QString();
				}
			}
			if (codec == NULL) {
				if (m_detectCodec)
					codec = detectCodec();
				else
					codec = asciiCodec();
			}
			QString message = codec->toUnicode(message_data);
			debug() << "New message has been received on channel 2:" << message;
			return message;
		} else if (MsgPlugin) {
			data.skipData(3);
			DataUnit info = data.read<DataUnit, quint16>(LittleEndian);
			Capability pluginType = info.read<Capability>();
			quint16 pluginId = info.read<quint16>(LittleEndian);
			QString pluginName = info.read<QString, quint32>(LittleEndian);
			DataUnit pluginData = data.read<DataUnit, quint32>(LittleEndian);
			if (pluginType.isNull()) {
				if (ack == 2) {
					ChatSession *session = ChatLayer::instance()->getSession(contact, false);
					if (session) {
						QApplication::instance()->postEvent(session, new MessageReceiptEvent(msgCookie.id(), true));
						debug() << "Message with id" << msgCookie.id() << "has been delivered";
					}
				}
			} else {
				bool found = false;
				foreach (Tlv2711Plugin *plugin, m_tlvs2711Plugins.values(Tlv2711Type(pluginType, pluginId))) {
					plugin->processTlvs2711(contact, pluginType, pluginId, pluginData, msgCookie);
					found = true;
				}
				if (!found) {
					debug() << "Unhandled plugin message" << pluginType.toString()
							<< pluginId << pluginName << pluginData.data().toHex();
				}
			}
		} else
			debug() << "Unhandled TLV 2711 message with type" << hex << type;
	} else
		debug() << "Unknown format of TLV 2711";
	return QString();
}

void MessagesHandler::sendChannel2Response(IcqContact *contact, quint8 type, quint8 flags, const Cookie &cookie)
{
	Tlv2711 responseTlv(type, flags, 0, 0);
	responseTlv.appendEmptyPacket();
	responseTlv.appendColors();
	ServerResponseMessage response(contact, 2, 3, cookie);
	response.append(responseTlv.data());
	m_account->connection()->send(response);
}

void MessagesHandler::sendMetaInfoRequest(quint16 type)
{
	SNAC snac(ExtensionsFamily, ExtensionsMetaCliRequest);
	DataUnit data;
	data.append<quint16>(8, LittleEndian); // data chunk size
	data.append<quint32>(m_account->id().toUInt(), LittleEndian);
	data.append<quint16>(type, LittleEndian); // message request cmd
	data.append<quint16>(snac.id()); // request sequence number
	snac.appendTLV(0x01, data);
	m_account->connection()->send(snac);
}

MessagePlugin::~MessagePlugin()
{
}

Tlv2711Plugin::~Tlv2711Plugin()
{
}

} } // namespace qutim_sdk_0_3::oscar

/****************************************************************************
 *  roster.cpp
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

#include "roster.h"
#include "icqcontact_p.h"
#include <icqaccount.h>
#include <qutim/objectgenerator.h>
#include <qutim/contactlist.h>
#include <QTextCodec>
#include <QDateTime>

struct SSIItem
{
public:
	SSIItem(const SNAC &snac);
	~SSIItem(){}
	QString toString();
	QString record_name;
	quint16 group_id;
	quint16 item_id;
	quint16 item_type;
	TLVMap tlvs;
};

SSIItem::SSIItem(const SNAC &snac)
{
	record_name = snac.readString<quint16>();
	group_id = snac.readSimple<quint16>();
	item_id = snac.readSimple<quint16>();
	item_type = snac.readSimple<quint16>();
	tlvs = DataUnit(snac.readData<quint16>()).readTLVChain();
}

QString SSIItem::toString()
{
	QString buf;
	QTextStream out(&buf, QIODevice::WriteOnly);
	out << record_name << " "
			<< group_id << " " << item_id << " "
			<< item_type << " (";
	bool first = true;
	foreach(const TLV &tlv, tlvs)
	{
		if(!first)
			out << ", ";
		else
			first = false;
		out << "0x" << QString::number(tlv.type(), 16);
	}
	out << ")";
	return qPrintable(buf);
}

Roster::Roster(IcqAccount *account)
{
	m_account = account;
	m_infos << SNACInfo(ListsFamily, ListsError)
			<< SNACInfo(ListsFamily, ListsList)
			<< SNACInfo(ListsFamily, ListsUpdateGroup)
			<< SNACInfo(ListsFamily, ListsCliModifyStart)
			<< SNACInfo(ListsFamily, ListsCliModifyEnd)
			<< SNACInfo(ListsFamily, ListsAuthRequest)
			<< SNACInfo(BuddyFamily, UserOnline)
			<< SNACInfo(BuddyFamily, UserOffline)
			<< SNACInfo(MessageFamily, MessageSrvRecv)
			<< SNACInfo(MessageFamily, MessageSrvAck)
			<< SNACInfo(MessageFamily, MessageSrvError)
			<< SNACInfo(ExtensionsFamily, ExtensionsMetaError)
			<< SNACInfo(ExtensionsFamily, ExtensionsMetaSrvReply);
	m_state = ReceivingRoster;
	foreach(const ObjectGenerator *gen, moduleGenerators<MessagePlugin>())
	{
		MessagePlugin *plugin = gen->generate<MessagePlugin>();
		foreach(const Capability &cap, plugin->capabilities())
			m_msg_plugins.insert(cap, plugin);
	}
}

void Roster::handleSNAC(OscarConnection *conn, const SNAC &sn)
{
	switch((sn.family() << 16) | sn.subtype())
	{
		// Server sends contactlist
		case ListsFamily << 16 | ListsList:
			handleServerCListReply(conn, sn);
			break;
		// Server sends contact list updates
		case ListsFamily << 16 | ListsUpdateGroup:
			handleCListUpdates(conn, sn);
			break;
		case ListsFamily << 16 | ListsCliModifyStart:
			qDebug() << IMPLEMENT_ME << "ListsCliModifyStart";
			break;
		case ListsFamily << 16 | ListsCliModifyEnd:
			qDebug() << IMPLEMENT_ME << "ListsCliModifyEnd";
			break;
		case ListsFamily << 16 | ListsAuthRequest: {
			sn.skipData(8); // unknown 8 bytes. Maybe cookie?
			QString uin = sn.readString<quint8>();
			QString reason = sn.readString<qint16>();
			qDebug() << QString("Authorization request from \"%1\" with reason \"%2").arg(uin).arg(reason);
			break;
		}
		case BuddyFamily << 16 | UserOnline:
			handleUserOnline(conn, sn);
			break;
		case BuddyFamily << 16 | UserOffline:
			handleUserOffline(conn, sn);
			break;
		case MessageFamily << 16 | MessageSrvRecv:
			handleMessage(conn, sn);
			break;
		case MessageFamily << 16 | MessageSrvAck: {
			sn.skipData(8); // skip cookie.
			quint16 channel = sn.readSimple<quint16>();
			QString uin = sn.readString<qint8>();
			qDebug() << QString("Server accepted message for delivery to %1 on channel %2").
					arg(uin).arg(channel);
			break;
		}
		case ListsFamily << 16 | ListsError:
		case MessageFamily << 16 | MessageSrvError:
		case ExtensionsFamily << 16 | ExtensionsMetaError:
			handleError(conn, sn);
			break;
		case ExtensionsFamily << 16 | ExtensionsMetaSrvReply:
			handleMetaInfo(conn, sn);
			break;
	}
}

void Roster::sendMessage(OscarConnection *conn, const QString &id, const QString &message)
{
	SNAC sn(MessageFamily, MessageSrvSend);
	sn.appendSimple<qint64>(QDateTime::currentDateTime().toTime_t()); // cookie
	sn.appendSimple<qint16>(0x0001); // message channel
	sn.appendData<qint8>(asciiCodec()->fromUnicode(id)); // uid or screenname

	DataUnit msgData;
	// Charset.
	// TODO: get supported charsets from client info.
	// 0x0000 - us-ascii
	// 0x0002 - utf-16 be
	// 0x0003 - ansi
	msgData.appendSimple<quint16>(0x0002);
	// Message.
	msgData.appendData(utf16Codec()->fromUnicode(message));

	DataUnit dataUnit;
	dataUnit.appendTLV(0x0501, (quint32)0x0106);
	dataUnit.appendTLV(0x0101, msgData);

	sn.appendTLV(0x0002, dataUnit);
	// empty TLV(6) store message if recipient offline.
	sn.appendTLV(0x0006);
	conn->send(sn);
}

void Roster::sendAuthResponse(OscarConnection *conn, const QString &id, const QString &message, bool auth)
{
	SNAC snac(ListsFamily, ListsCliAuthResponse);
	snac.appendData<qint8>(asciiCodec()->fromUnicode(id)); // uin.
	snac.appendSimple<qint8>(auth ? 0x01 : 0x00); // auth flag.
	snac.appendData<qint16>(asciiCodec()->fromUnicode(message)); // TODO: which codec should be used?
	conn->send(snac);
}

void Roster::handleServerCListReply(OscarConnection *conn, const SNAC &sn)
{
	Q_UNUSED(conn);
	if(!(sn.flags() & 0x0001))
		m_state = RosterReceived;
	quint8 version = sn.readSimple<quint8>();
	quint16 count = sn.readSimple<quint16>();
	bool is_last = !(sn.flags() & 0x0001);
	qDebug("SSI: number of entries is %u, version is %u", (uint)count, (uint)version);
	for(uint i = 0; i < count; i++)
	{
		SSIItem item(sn);
		switch(item.item_type)
		{
		case SsiBuddy: {
			IcqContact *contact = 0;
			// record name contains uin, while item id contains some random value
			if(!(contact = m_contacts.value(item.record_name)))
				m_contacts.insert(item.record_name, contact = new IcqContact(item.record_name, m_account));
			bool not_auth = false;
			if(item.tlvs.contains(0x0131))
				contact->p->name = item.tlvs.value<QString>(0x0131);
			if(item.tlvs.contains(0x013c))
				contact->setProperty("comment", item.tlvs.value<QString>(0x013c));
			if(item.tlvs.contains(0x0066))
				not_auth = true;
			contact->p->group_id = item.group_id;
			qDebug() << contact->id() << contact->name();
#ifndef TEST
			ContactList::instance()->addContact(contact);
#endif //TEST
			break; }
		case SsiGroup:
			if(item.group_id > 0)
			{
				// record name contains name of group
				m_groups.insert(item.group_id, item.record_name);
			}
			else
			{
			}
			break;
		default:
			qDebug() << QString("Dump of unknown SSI item: %1").arg(item.toString());
		}
	}
	qDebug() << "is_last" << is_last;
	if(is_last)
	{
		quint32 last_info_update = sn.readSimple<quint32>();
		qDebug() << last_info_update;
		conn->setProperty("SrvLastUpdate", last_info_update);
		sendRosterAck(conn);
		conn->finishLogin();
		sendOfflineMessagesRequest(conn);
	}
	qDebug() << m_groups;
}

void Roster::handleCListUpdates(OscarConnection *conn, const SNAC &sn)
{
	while(sn.dataSize() != 0)
	{
		SSIItem item(sn);
		switch(item.item_type)
		{
		case SsiBuddy: {
			IcqContact *contact = m_contacts.value(item.record_name);
			if(!contact)
				return;
			/*if(tlvs.contains(0x0131))
				contact->p->name = tlvs.value<QString>(0x0131);
			if(tlvs.contains(0x013c))
				contact->setProperty("comment", tlvs.value<QString>(0x013c));*/
			bool auth = !item.tlvs.contains(0x0066);
			qDebug() << IMPLEMENT_ME << "update the contact";
			break; }
		default:
			qDebug() << QString("Dump of unknown SSI item: %1").arg(item.toString());
		}
	}
}

void Roster::handleUserOnline(OscarConnection *, const SNAC &snac)
{
	QString uin = snac.readData<quint8>();
	IcqContact *contact = m_contacts.value(uin, 0);
	// We don't know this contact
	if(!contact)
		return;
	quint16 warning_level = snac.readSimple<quint16>();
	TLVMap tlvs = snac.readTLVChain<quint16>();
	if(tlvs.contains(0x000c))
	{
		DataUnit data(tlvs.value(0x000c));
		DirectConnectionInfo info =
		{
			QHostAddress(data.readSimple<quint32>()),
			data.readSimple<quint32>(),
			data.readSimple<quint8>(),
			data.readSimple<quint16>(),
			data.readSimple<quint32>(),
			data.readSimple<quint32>(),
			data.readSimple<quint32>(),
			data.readSimple<quint32>(),
			data.readSimple<quint32>(),
			data.readSimple<quint32>()
		};
	}
	qDebug("Handle UserOnline %02X %02X, %s", (int)snac.family(), (int)snac.subtype(), qPrintable(uin));
	qDebug() << tlvs.keys();
	quint32 status = tlvs.value<quint32>(0x0006, 0x0000);
	contact->p->status = icqStatusToQutim(status & 0xffff);
	emit contact->statusChanged(contact->p->status);
	qDebug("%s", qPrintable(QString::number(status, 16)));
//	bool status_present = tlv06.type() == 0x0006;
}

void Roster::handleUserOffline(OscarConnection *, const SNAC &snac)
{
	QString uin = snac.readString<quint8>();
	IcqContact *contact = m_contacts.value(uin, 0);
	// We don't know this contact
	if(!contact)
		return;
	contact->p->status = Offline;
	contact->statusChanged(contact->p->status);
//	quint16 warning_level = snac.readSimple<quint16>();
//	TLVMap tlvs = snac.readTLVChain<quint16>();
//	tlvs.value(0x0001); // User class
}

void Roster::handleMessage(OscarConnection *conn, const SNAC &snac)
{
	quint64 cookie = snac.readSimple<quint64>();
	quint16 channel = snac.readSimple<quint16>();
	QString uin = snac.readString<quint8>();
	quint16 warning = snac.readSimple<quint16>();
	snac.skipData(2); // unused number of tlvs
	TLVMap tlvs = snac.readTLVChain();
	QString message;
	QDateTime time;
	qDebug() << "channel" << channel << uin << tlvs.keys();
	switch(channel)
	{
	case 0x0001: // message
		if(tlvs.contains(0x0002))
		{
			DataUnit data(tlvs.value(0x0002));
			TLVMap msg_tlvs = data.readTLVChain();
			if(msg_tlvs.contains(0x0501))
				qDebug("Message has %s caps", msg_tlvs.value(0x0501).value().toHex().constData());
			foreach(const TLV &tlv, msg_tlvs.values(0x0101))
			{
				DataUnit msg_data(tlv);
				quint16 charset = msg_data.readSimple<quint16>();
				// 0x0000 - us-ascii
				// 0x0002 - utf-16 be
				// 0x0003 - ansi
				quint16 codepage = msg_data.readSimple<quint16>();
				QByteArray data = msg_data.readAll();
				QTextCodec *codec = 0;
				if(charset == 0x0002)
					codec = utf16Codec();
				else
					codec = asciiCodec();
				message += codec->toUnicode(data);
			}
			if(!(snac.id() & 0x80000000) && msg_tlvs.contains(0x0016)) // Offline message
				time = QDateTime::fromTime_t(msg_tlvs.value(0x0016).value<quint32>());
		}
		break;
	case 0x0002: // rendezvous
		if(tlvs.contains(0x0005))
		{
			DataUnit data(tlvs.value(0x0005));
			quint16 type = data.readSimple<quint16>();
			data.skipData(8); // again cookie
			Capability guid = data.readCapability();
			if(guid.isEmpty())
			{
				qDebug() << "Incorrect message on channel 2 from" << uin << ": guid is not found";
				return;
			}
			QList<MessagePlugin *> plugins = m_msg_plugins.values(guid);
			if(!plugins.isEmpty())
			{
				QByteArray plugin_data = data.readAll();
				for(int i = 0; i < plugins.size(); i++)
					plugins.at(i)->processMessage(uin, guid, plugin_data, cookie);
			}
			else
				qDebug() << IMPLEMENT_ME << QString("Message (channel 2) from %1 with type %2 is not processed.").
					arg(uin).arg(type);
		}
		else
			qDebug() << "Incorrect message on channel 2 from" << uin << ": SNAC should contain TLV 5";
		break;
	case 0x0004:
		// TODO: Understand this holy shit
		if(tlvs.contains(0x0005))
		{
			DataUnit data(tlvs.value(0x0005));
			quint32 uin_2 = data.readSimple<quint32>(DataUnit::LittleEndian);
			if(QString::number(uin_2) != uin)
				return;
			quint8 type = data.readSimple<quint8>();
			quint8 flags = data.readSimple<quint8>();
			QByteArray msg_data = data.readData<quint16>(DataUnit::LittleEndian);
			qDebug() << IMPLEMENT_ME << QString("Message (channel 3) from %1 with type %2 is not processed.").
					arg(uin).arg(type);
		}
		else
			qDebug() << "Incorrect message on channel 3 from" << uin << ": SNAC should contain TLV 5";
		break;
	default:
		qWarning("Unknown message channel: %d", int(channel));
	}
	if(!message.isEmpty())
	{
		if(!time.isValid())
			time = QDateTime::currentDateTime();
		qDebug() << "Received message at" << time << message;
#ifdef TEST
		sendMessage(conn, uin, "Autoresponse: your message is \"" + message + "\"");
#endif // TEST
	}
}

void Roster::handleError(OscarConnection *conn, const SNAC &snac)
{
	qint16 errorCode = snac.readSimple<qint16>();
	qint16 subcode = 0;
	QString error;
	TLVMap tlvs = snac.readTLVChain();
	if(tlvs.contains(0x08))
	{
		DataUnit data(tlvs.value(0x08));
		subcode = data.readSimple<qint16>();
	}
	switch(errorCode)
	{
		case(0x01):
			error = "Invalid SNAC header";
			break;
		case(0x02):
			error = "Server rate limit exceeded";
			break;
		case(0x03):
			error = "Client rate limit exceeded";
			break;
		case(0x04):
			error = "Recipient is not logged in";
			break;
		case(0x05):
			error = "Requested service unavailable";
			break;
		case(0x06):
			error = "Requested service not defined";
			break;
		case(0x07):
			 error = "You sent obsolete SNAC";
			 break;
		case(0x08):
			error = "Not supported by server";
			break;
		case(0x09):
			error = "Not supported by client";
			break;
		case(0x0A):
			error = "Refused by client";
			break;
		case(0x0B):
			error = "Reply too big";
			break;
		case(0x0C):
			error = "Responses lost";
			break;
		case(0x0D):
			error = "Request denied";
			break;
		case(0x0E):
			error = "Incorrect SNAC format";
			break;
		case(0x0F):
			error = "Insufficient rights";
			break;
		case(0x10):
			error = "In local permit/deny (recipient blocked)";
			break;
		case(0x11):
			error = "Sender too evil";
			break;
		case(0x12):
			error = "Receiver too evil";
			break;
		case(0x13):
			error = "User temporarily unavailable";
			break;
		case(0x14):
			error = "No match";
			break;
		case(0x15):
			error = "List overflow";
			break;
		case(0x16):
			error = "Request ambiguous";
			break;
		case(0x17):
			error = "Server queue full";
			break;
		case(0x18):
			error = "Not while on AOL";
			break;
		default:
			error = "Unknown error";
	}
	qDebug() << QString("Error (%1, %2): %3").
			arg(errorCode, 2, 16).arg(subcode, 2, 16).arg(error);
}

void Roster::handleMetaInfo(OscarConnection *conn, const SNAC &snac)
{
	TLVMap tlvs = snac.readTLVChain();
	if(tlvs.contains(0x01))
	{
		DataUnit data(tlvs.value(0x01));
		data.skipData(6); // skip length field + my uin
		quint16 metaType = data.readSimple<quint16>(DataUnit::LittleEndian);
		switch(metaType)
		{
		case(0x0041):
			// Offline message.
			// It seems it's not used anymore.
			break;
		case(0x0042):
			// Delete offline messages from the server.
			sendMetaInfoRequest(conn, 0x003E);
			break;
		default:
			qDebug() << "Unhandled meta information response" << metaType;
		}
	}
}

void Roster::sendRosterAck(OscarConnection *conn)
{
	SNAC snac(ListsFamily, ListsGotList);
	conn->send(snac);
	qDebug("Send Roster Ack, SNAC %02X %02X", (int)snac.family(), (int)snac.subtype());
}

void Roster::sendMetaInfoRequest(OscarConnection *conn, quint16 type)
{
	SNAC snac(ExtensionsFamily, ExtensionsMetaCliRequest);
	DataUnit data;
	data.appendSimple<quint16>(8, DataUnit::LittleEndian); // data chunk size
	data.appendSimple<quint32>(conn->account()->id().toUInt(), DataUnit::LittleEndian);
	data.appendSimple<quint16>(type, DataUnit::LittleEndian); // message request cmd
	data.appendSimple<quint16>(snac.id()); // request sequence number
	snac.appendTLV(0x01, data);
	conn->send(snac);
}

QTextCodec *Roster::asciiCodec()
{
	static QTextCodec *codec =  QTextCodec::codecForName("cp1251");
	return codec;
}

QTextCodec *Roster::utf16Codec()
{
	static QTextCodec *codec = QTextCodec::codecForName("UTF-16BE");
	return codec;
}

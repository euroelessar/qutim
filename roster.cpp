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
#include <qutim/objectgenerator.h>
#include <qutim/contactlist.h>
#include <QTextCodec>
#include <QDateTime>

Roster::Roster(IcqAccount *account)
{
	m_account = account;
	m_infos << SNACInfo(ListsFamily, ListsList)
			<< SNACInfo(BuddyFamily, UserOnline)
			<< SNACInfo(BuddyFamily, UserOffline)
			<< SNACInfo(MessageFamily, MessageSrvRecv);
	m_state = ReceivingRoster;
	foreach(const ObjectGenerator *gen, moduleGenerators<MessagePlugin>())
	{
		MessagePlugin *plugin = gen->generate<MessagePlugin>();
		foreach(const QByteArray &cap, plugin->capabilities())
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
		case BuddyFamily << 16 | UserOnline:
			handleUserOnline(conn, sn);
		break;
		case BuddyFamily << 16 | UserOffline:
			handleUserOffline(conn, sn);
		break;
		case MessageFamily << 16 | MessageSrvRecv:
			handleMessage(conn, sn);
	}
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
		QString record_name = sn.readString<quint16>();
		quint16 group_id = sn.readSimple<quint16>();
		quint16 item_id = sn.readSimple<quint16>();
		quint16 item_type = sn.readSimple<quint16>();
		const TLVMap tlvs = DataUnit(sn.readData<quint16>()).readTLVChain();
		switch(item_type)
		{
		case SsiBuddy: {
			IcqContact *contact = 0;
			// record name contains uin, while item id contains some random value
			if(!(contact = m_contacts.value(record_name)))
				m_contacts.insert(record_name, contact = new IcqContact(record_name, m_account));
			bool not_auth = false;
			if(tlvs.contains(0x0131))
				contact->p->name = tlvs.value<QString>(0x0131);
			if(tlvs.contains(0x013c))
				contact->setProperty("comment", tlvs.value<QString>(0x013c));
			if(tlvs.contains(0x0066))
				not_auth = true;
			contact->p->group_id = group_id;
			qDebug() << contact->id() << contact->name();
			ContactList::instance()->addContact(contact);
			break; }
		case SsiGroup:
			if(group_id > 0)
			{
				// record name contains name of group
				m_groups.insert(group_id, record_name);
			}
			else
			{
			}
			break;
		}
		QString buf;
		{
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
		}
		qDebug("%s", qPrintable(buf));
	}
	qDebug() << "is_last" << is_last;
	if(is_last)
	{
		quint32 last_info_update = sn.readSimple<quint32>();
		qDebug() << last_info_update;
		conn->setProperty("SrvLastUpdate", last_info_update);
		sendRosterAck(conn);
		conn->finishLogin();
	}
	qDebug() << m_groups;
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
	static QTextCodec *utf16_codec = QTextCodec::codecForName("UTF-16BE");
	static QTextCodec *ascii_codec = QTextCodec::codecForName("cp1251");
	QString message;
	QDateTime time;
	qDebug() << "channel" << channel << uin << tlvs.keys();
	switch(channel)
	{
	case 0x0001:
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
					codec = utf16_codec;
				else
					codec = ascii_codec;
				message += codec->toUnicode(data);
			}
			if(!(snac.id() & 0x80000000) && msg_tlvs.contains(0x0016)) // Offline message
				time = QDateTime::fromTime_t(msg_tlvs.value(0x0016).value<quint32>());
		}
		break;
	case 0x0002:
		if(tlvs.contains(0x0005))
		{
			DataUnit data(tlvs.value(0x0005));
			quint16 type = data.readSimple<quint16>();
			data.skipData(8); // again cookie
			QByteArray guid = data.readData(0x10);\
			if(guid.size() != 0x10)
				return;
			QList<MessagePlugin *> plugins = m_msg_plugins.values(guid);
			QByteArray plugin_data = data.readAll();
			for(int i = 0; i < plugins.size(); i++)
				plugins.at(i)->processMessage(uin, guid, plugin_data, cookie);
		}
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
		}
		break;
	default:
		qWarning("Unknown message channel: %d", int(channel));
	}
	if(!message.isEmpty())
	{
		if(!time.isValid())
			time = QDateTime::currentDateTime();
		qDebug() << "Received message at" << time << message;
	}
}

void Roster::sendRosterAck(OscarConnection *conn)
{
	SNAC snac(ListsFamily, ListsGotList);
	conn->send(snac);
	qDebug("Send Roster Ack, SNAC %02X %02X", (int)snac.family(), (int)snac.subtype());
}

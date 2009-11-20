/****************************************************************************
 *  roster.cpp
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#include "roster.h"
#include "icqcontact_p.h"
#include <icqaccount.h>
#include <qutim/objectgenerator.h>
#include <qutim/contactlist.h>
#include <qutim/messagesession.h>
#include <QTextCodec>
#include <QDateTime>

using namespace Util;

Roster::SSIItem::SSIItem(const SNAC &snac)
{
	record_name = snac.readString<quint16>();
	group_id = snac.readSimple<quint16>();
	item_id = snac.readSimple<quint16>();
	item_type = snac.readSimple<quint16>();
	tlvs = DataUnit(snac.readData<quint16>()).readTLVChain();
}

QString Roster::SSIItem::toString() const
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
	m_conn = account->connection();
	m_infos << SNACInfo(ListsFamily, ListsError)
			<< SNACInfo(ListsFamily, ListsList)
			<< SNACInfo(ListsFamily, ListsUpdateGroup)
			<< SNACInfo(ListsFamily, ListsAddToList)
			<< SNACInfo(ListsFamily, ListsRemoveFromList)
			<< SNACInfo(ListsFamily, ListsAck)
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

void Roster::handleSNAC(OscarConnection *c, const SNAC &sn)
{
	Q_ASSERT(c == m_conn);
	switch((sn.family() << 16) | sn.subtype())
	{
		// Server sends contactlist
		case ListsFamily << 16 | ListsList:
			handleServerCListReply(sn);
			break;
		// Server sends contact list updates
		case ListsFamily << 16 | ListsUpdateGroup:
		// Server sends new items
		case ListsFamily << 16 | ListsAddToList:
		// Items was removed
		case ListsFamily << 16 | ListsRemoveFromList:
			while(sn.dataSize() != 0)
			{
				SSIItem item(sn);
				handleSSIItem(item, (ModifingType)sn.subtype());
			}
			break;
		case ListsFamily << 16 | ListsAck:
			handleSSIServerAck(sn);
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
			handleUserOnline(sn);
			break;
		case BuddyFamily << 16 | UserOffline:
			handleUserOffline(sn);
			break;
		case MessageFamily << 16 | MessageSrvRecv:
			handleMessage(sn);
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
			handleError(sn);
			break;
		case ExtensionsFamily << 16 | ExtensionsMetaSrvReply:
			handleMetaInfo(sn);
			break;
	}
}

void Roster::sendMessage(const QString &id, const QString &message)
{
	SNAC sn(MessageFamily, MessageSrvSend);
	sn.appendSimple<qint64>(QDateTime::currentDateTime().toTime_t()); // cookie
	sn.appendSimple<qint16>(0x0001); // message channel
	sn.appendData<qint8>(id); // uid or screenname

	DataUnit msgData;
	// Charset.
	// TODO: get supported charsets from client info.
	// 0x0000 - us-ascii
	// 0x0002 - utf-16 be
	// 0x0003 - ansi
	msgData.appendSimple<quint16>(0x0002);
	// Message.
	msgData.appendData(message, utf16Codec());

	DataUnit dataUnit;
	dataUnit.appendTLV(0x0501, (quint32)0x0106);
	dataUnit.appendTLV(0x0101, msgData);

	sn.appendTLV(0x0002, dataUnit);
	// empty TLV(6) store message if recipient offline.
	sn.appendTLV(0x0006);
	m_conn->send(sn);
}

void Roster::sendAuthResponse(const QString &id, const QString &message, bool auth)
{
	SNAC snac(ListsFamily, ListsCliAuthResponse);
	snac.appendData<qint8>(id); // uin.
	snac.appendSimple<qint8>(auth ? 0x01 : 0x00); // auth flag.
	snac.appendData<qint16>(message); // TODO: which codec should be used?
	m_conn->send(snac);
}

void Roster::sendAddGroupRequest(const QString &name, quint16 group_id)
{
	SSIItem item;
	item.item_type = SsiGroup;
	item.record_name = name;
	if(group_id == 0)
	{
		group_id = rand() % 0x03e6;
		while(m_groups.contains(0x03e6))
			group_id = rand() % 0x03e6;
	}
	item.group_id = group_id;
	item.tlvs.insert(0x00c8);

	sendCLModifyStart();
	sendCLOperator(item, ListsAddToList);
	sendCLModifyEnd();
}

void Roster::sendRemoveGroupRequest(quint16 id)
{
	SSIItem item;
	item.item_type = SsiGroup;
	item.group_id = id;

	sendCLModifyStart();
	sendCLOperator(item, ListsRemoveFromList);
	sendCLModifyEnd();
}

IcqContact *Roster::sendAddContactRequest(const QString &contact_id, const QString &contact_name, quint16 group_id)
{
	SSIItem item;
	item.item_type = SsiBuddy;
	item.record_name = contact_id;
	item.group_id = group_id;
	// Generating user_id
	quint16 id = rand() % 0x03e6;
	forever
	{
		bool found = false;
		foreach(IcqContact *contact, m_contacts)
		{
			if(contact->p->user_id == id)
			{
				found = true;
				break;
			}
		}
		if(found)
			id = rand() % 0x03e6;
		else
			break;
	}
	item.item_id = id;
	item.tlvs.insert(0x0131, asciiCodec()->fromUnicode(contact_name)); // nick
	item.tlvs.insert(0x0066); // auth flag

	sendCLModifyStart();
	sendCLOperator(item, ListsAddToList);
	sendCLModifyEnd();

	IcqContact *contact = new IcqContact(contact_id, m_account);
	m_not_in_list.insert(contact_id, contact);
	return contact;

}

void Roster::sendRemoveContactRequst(const QString &contact_id)
{
	IcqContact *contact = m_contacts.value(contact_id);
	if(contact)
	{
		SSIItem item;
		item.item_type = SsiBuddy;
		item.item_id = contact->p->user_id;
		item.group_id = contact->p->group_id;
		item.record_name = contact->id();

		sendCLModifyStart();
		sendCLOperator(item, ListsRemoveFromList);
		sendCLModifyEnd();
	}
	else
		qDebug() << Q_FUNC_INFO << QString("The contact (%1) does not exist").arg(contact_id);
}

void Roster::handleServerCListReply(const SNAC &sn)
{
	if(!(sn.flags() & 0x0001))
		m_state = RosterReceived;
	quint8 version = sn.readSimple<quint8>();
	quint16 count = sn.readSimple<quint16>();
	bool is_last = !(sn.flags() & 0x0001);
	qDebug("SSI: number of entries is %u, version is %u", (uint)count, (uint)version);
	for(uint i = 0; i < count; i++)
	{
		SSIItem item(sn);
		handleSSIItem(item, mt_add_modify);
	}
	qDebug() << "is_last" << is_last;
	if(is_last)
	{
		quint32 last_info_update = sn.readSimple<quint32>();
		qDebug() << "SrvLastUpdate" << last_info_update;
		m_conn->setProperty("SrvLastUpdate", last_info_update);
		sendRosterAck();
		m_conn->finishLogin();
		sendOfflineMessagesRequest();
		if(!m_groups.contains(not_in_list_group))
			sendAddGroupRequest(tr("Not in list"), not_in_list_group);
	}
}

void Roster::handleSSIItem(const SSIItem &item, ModifingType type)
{
	if(type == mt_remove)
		handleRemoveCLItem(item);
	else
		handleAddModifyCLItem(item, type);
}

void Roster::handleAddModifyCLItem(const SSIItem &item, ModifingType type)
{
	Q_ASSERT(type != mt_remove);
	switch(item.item_type)
	{
	case SsiBuddy: {
		IcqContact *contact = m_contacts.value(item.record_name);
		// record name contains uin
		bool is_adding = !contact;
		if(is_adding && type == mt_modify)
		{
			qDebug() << Q_FUNC_INFO << "Contact does not exist" << item.record_name;
			return;
		}
		if(!is_adding && type == mt_add)
		{
			qDebug() << Q_FUNC_INFO << "Contact already is in contactlist";
			return;
		}
		if(is_adding)
		{
			contact = m_not_in_list.take(item.record_name);
			if(!contact)
				contact = new IcqContact(item.record_name, m_account);
			m_contacts.insert(item.record_name, contact);
			contact->p->group_id = item.group_id;
			if(item.tlvs.contains(0x0131))
				contact->p->name = item.tlvs.value<QString>(0x0131);
			if(item.tlvs.contains(0x013c))
				contact->setProperty("comment", item.tlvs.value<QString>(0x013c));
			bool auth = !item.tlvs.contains(0x0066);
			// TODO: update the auth field in the contact
			if(ContactList::instance())
				ContactList::instance()->addContact(contact);
			qDebug() << "New contact is added" << contact->id() << contact->name() << item.item_id;
		}
		else
		{
			// name
			QString new_name = item.tlvs.value<QString>(0x0131);
			if(!new_name.isEmpty() && new_name != contact->p->name)
			{
				contact->p->name = new_name;
				emit contact->nameChanged(new_name);
			}
			// comment
			QString new_comment = item.tlvs.value<QString>(0x013c);
			if(!new_comment.isEmpty() && new_comment != contact->property("comment").toString())
			{
				contact->setProperty("comment", new_comment);
				// TODO: emit ...
			}
			// auth
			bool new_auth = !item.tlvs.contains(0x0066);
			// TODO: update contact
			qDebug() << "The contact is updated" << contact->id() << contact->name() << item.item_id;
		}
		contact->p->user_id = item.item_id;
		break; }
	case SsiGroup:
		if(item.group_id > 0)
		{
			// record name contains name of group
			QMap<quint16, QString>::iterator itr = m_groups.find(item.group_id);
			if(itr == m_groups.end())
			{
				if(type != mt_modify)
					m_groups.insert(item.group_id, item.record_name);
				else
					qDebug() << Q_FUNC_INFO << "Group does not exist" << item.group_id << item.record_name;
			}
			else
			{
				if(type != mt_add)
					itr.value() = item.record_name;
				else
					qDebug() << Q_FUNC_INFO << "Group already is in contactlist" << item.group_id << item.record_name;
			}
			qDebug() << "New group is added" << item.group_id << item.record_name;
		}
		else
		{
		}
		break;
	default:
		qDebug() << Q_FUNC_INFO << QString("Dump of unknown SSI item: %1").arg(item.toString());
	}
}

void Roster::handleRemoveCLItem(const SSIItem &item)
{
	switch(item.item_type)
	{
	case SsiBuddy: {
		QHash<QString, IcqContact *>::iterator contact_itr = m_contacts.begin();
		QHash<QString, IcqContact *>::iterator end_itr = m_contacts.end();
		while(contact_itr != end_itr)
		{
			if(contact_itr.value()->p->user_id == item.item_id)
				break;
			++contact_itr;
		}
		if(contact_itr == end_itr)
		{
			qDebug() << Q_FUNC_INFO << "contact does not exist" << item.item_id << item.record_name;
			break;
		}
		removeContact(*contact_itr);
		m_contacts.erase(contact_itr);
		qDebug() << "contact is removed" << item.item_id << item.record_name;
		break;
	}
	case SsiGroup: {
		QMap<quint16, QString>::iterator group_itr = m_groups.find(item.group_id);
		if(group_itr == m_groups.end())
		{
			qDebug() << Q_FUNC_INFO << "group does not exist" << item.group_id;
			break;
		}
		// Removing all contacts in the group.
		QHash<QString, IcqContact *>::iterator contact_itr = m_contacts.begin();
		QHash<QString, IcqContact *>::iterator contact_end_itr = m_contacts.end();
		while(contact_itr != contact_end_itr)
		{
			if(contact_itr.value()->p->group_id == item.group_id)
			{
				removeContact(*contact_itr);
				contact_itr = m_contacts.erase(contact_itr);
			}
			else
				++contact_itr;
		}
		// Removing the group.
		m_groups.erase(group_itr);
		qDebug() << "group is removed" << item.group_id;
		break;
	}
	default:
		qDebug() << Q_FUNC_INFO << QString("Dump of unknown SSI item: %1").arg(item.toString());
	}

}

void Roster::removeContact(IcqContact *contact)
{
	if(ContactList::instance())
		ContactList::instance()->removeContact(contact);
	delete contact;
}

void Roster::handleSSIServerAck(const SNAC &sn)
{
	sn.skipData(8); // cookie?
	while(sn.dataSize() != 0)
	{
		quint16 error = sn.readSimple<quint16>();
		if(error == 0)
		{
			SSIHistoryItem operation = m_ssi_history.dequeue();
			qDebug() << "The last SSI operation is successfully done" << operation.type
					<< operation.item.item_type << operation.item.record_name << operation.item.item_id
					<< operation.item.group_id;
			handleSSIItem(operation.item, operation.type);
			break;
		}
		QString error_str;
		if(error ==  0x0002)
			error_str = "Item you want to modify not found in list";
		else if(error ==  0x0003)
			error_str = "Item you want to add allready exists";
		else if(error ==  0x000a)
			error_str = "Error adding item (invalid id, allready in list, invalid data)";
		else if(error ==  0x000c)
			error_str = "Can't add item. Limit for this type of items exceeded";
		else if(error ==  0x000d)
			error_str = "Trying to add ICQ contact to an AIM list";
		else if(error ==  0x000e)
			error_str = "Can't add this contact because it requires authorization";
		else
			error_str = QString::number(error);
		qDebug() << "SSI operation error" << error_str;
	}
}

void Roster::handleUserOnline(const SNAC &snac)
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

void Roster::handleUserOffline(const SNAC &snac)
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

void Roster::handleMessage(const SNAC &snac)
{
	quint64 cookie = snac.readSimple<quint64>();
	quint16 channel = snac.readSimple<quint16>();
	QString uin = snac.readString<quint8>();
	quint16 warning = snac.readSimple<quint16>();
	snac.skipData(2); // unused number of tlvs
	TLVMap tlvs = snac.readTLVChain();
	QString message;
	QDateTime time;
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
		qDebug() << "Received message at" << uin << time << message;
		if(ChatLayer::instance())
		{
			ChatSession *session = ChatLayer::instance()->getSession(m_account, uin);
			Message m;
			m.setIncoming(true);
			m.setText(message);
			m.setTime(time);
			m.setChatUnit(session->getUnit());
			session->appendMessage(m);
		}
#ifdef TEST
		sendMessage(uin, "Autoresponse: your message is \"" + message + "\"");
#endif // TEST
	}
}

void Roster::handleError(const SNAC &snac)
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

void Roster::handleMetaInfo(const SNAC &snac)
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
			sendMetaInfoRequest(0x003E);
			break;
		default:
			qDebug() << "Unhandled meta information response" << metaType;
		}
	}
}

void Roster::sendRosterAck()
{
	SNAC snac(ListsFamily, ListsGotList);
	m_conn->send(snac);
	qDebug("Send Roster Ack, SNAC %02X %02X", (int)snac.family(), (int)snac.subtype());
}

void Roster::sendMetaInfoRequest(quint16 type)
{
	SNAC snac(ExtensionsFamily, ExtensionsMetaCliRequest);
	DataUnit data;
	data.appendSimple<quint16>(8, DataUnit::LittleEndian); // data chunk size
	data.appendSimple<quint32>(m_account->id().toUInt(), DataUnit::LittleEndian);
	data.appendSimple<quint16>(type, DataUnit::LittleEndian); // message request cmd
	data.appendSimple<quint16>(snac.id()); // request sequence number
	snac.appendTLV(0x01, data);
	m_conn->send(snac);
}

void Roster::sendCLModifyStart()
{
	SNAC snac(ListsFamily, ListsCliModifyStart);
	m_conn->send(snac);
}

void Roster::sendCLModifyEnd()
{
	SNAC snac(ListsFamily, ListsCliModifyEnd);
	m_conn->send(snac);
}

void Roster::sendCLOperator(const SSIItem &item, quint16 operation)
{
	m_ssi_history.enqueue(SSIHistoryItem(item, (ModifingType)operation));
	SNAC snac(ListsFamily, operation);
	snac.appendData<quint16>(item.record_name);
	snac.appendSimple<quint16>(item.group_id);
	snac.appendSimple<quint16>(item.item_id);
	snac.appendSimple<quint16>(item.item_type);
	snac.appendSimple<quint16>(item.tlvs.valuesSize());
	snac.appendData(item.tlvs);
	m_conn->send(snac);
}

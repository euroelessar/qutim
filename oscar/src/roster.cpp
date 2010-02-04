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
#include "icqaccount.h"
#include "icqprotocol.h"
#include "oscarconnection.h"
#include "buddypicture.h"
#include "buddycaps.h"
#include "clientidentify.h"
#include "messages.h"
#include "xtraz.h"
#include <qutim/contactlist.h>
#include <qutim/messagesession.h>
#include <qutim/notificationslayer.h>

namespace Icq
{

using namespace Util;

struct SessionDataItem
{
	quint16 type;
	quint8 flags;
	QByteArray data;
};

class SessionDataItemMap: public QMultiMap<quint16, SessionDataItem>
{
public:
	SessionDataItemMap(const TLVMap &tlvs)
	{
		if (tlvs.contains(0x1D))
			parseData(DataUnit(tlvs.value(0x1D)));
	}

	SessionDataItemMap(const DataUnit &data)
	{
		parseData(data);
	}
private:
	void parseData(const DataUnit &data)
	{
		SessionDataItem item;
		while (data.dataSize() >= 4) {
			item.type = data.readSimple<quint16>();
			item.flags = data.readSimple<quint8>();
			item.data = data.readData<quint8>();
			insertMulti(item.type, item);
		}
	}
};

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
	QString str;
	QTextStream stream(&str);
	if (!record_name.isEmpty())
		stream << "Name: " << record_name << "; ";
	stream << "ID: " << item_id << "; " << "type: " << item_type << "; " << "group: " << group_id << "(";
	bool first = true;
	foreach(const TLV &tlv, tlvs)
	{
		if (!first)
			stream << ", ";
		else
			first = false;
		stream << "0x" << hex << tlv.type();
	}
	stream << ")";
	return str;
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
			<< SNACInfo(ListsFamily, ListsSrvAuthResponse)
			<< SNACInfo(ListsFamily, ListsSrvReplyLists)
			<< SNACInfo(ListsFamily, ListsUpToDate)
			<< SNACInfo(BuddyFamily, UserOnline)
			<< SNACInfo(BuddyFamily, UserOffline)
			<< SNACInfo(BuddyFamily, UserSrvReplyBuddy)
			<< SNACInfo(ExtensionsFamily, ExtensionsMetaError)
			<< SNACInfo(ExtensionsFamily, ExtensionsMetaSrvReply);
	m_state = ReceivingRoster;
}

void Roster::handleSNAC(AbstractConnection *c, const SNAC &sn)
{
	Q_ASSERT(c == m_conn);
	switch ((sn.family() << 16) | sn.subtype()) {
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
		while (sn.dataSize() != 0) {
			SSIItem item(sn);
			handleSSIItem(item, (ModifingType) sn.subtype());
		}
		break;
	case ListsFamily << 16 | ListsAck:
		handleSSIServerAck(sn);
		break;
	case ListsFamily << 16 | ListsAuthRequest: {
		sn.skipData(8); // cookie
		QString uin = sn.readString<quint8>();
		QString reason = sn.readString<qint16>();
		debug() << QString("Authorization request from \"%1\" with reason \"%2").arg(uin).arg(reason);
		break;
	}
	case ListsFamily << 16 | ListsSrvAuthResponse: {
		sn.skipData(8); // cookie
		QString uin = sn.readString<qint8>();
		bool is_accepted = sn.readSimple<qint8>();
		QString reason = sn.readString<qint16>();
		debug() << "Auth response" << uin << is_accepted << reason;
		break;
	}
	case BuddyFamily << 16 | UserOnline:
		handleUserOnline(sn);
		break;
	case BuddyFamily << 16 | UserOffline:
		handleUserOffline(sn);
		break;
	case BuddyFamily << 16 | UserSrvReplyBuddy:
		debug() << IMPLEMENT_ME << "BuddyFamily, UserSrvReplyBuddy";
		break;
	}
}

void Roster::sendAuthResponse(const QString &id, const QString &message, bool auth)
{
	SNAC snac(ListsFamily, ListsCliAuthResponse);
	snac.appendData<qint8>(id); // uin.
	snac.appendSimple<qint8>(auth ? 0x01 : 0x00); // auth flag.
	snac.appendData<qint16>(message);
	m_conn->send(snac);
}

void Roster::sendAuthRequest(const QString &id, const QString &message)
{
	SNAC snac(ListsFamily, ListsRequestAuth);
	snac.appendData<qint8>(id); // uin.
	snac.appendData<qint16>(message);
	snac.appendSimple<quint16>(0);
	m_conn->send(snac);
}

quint16 Roster::sendAddGroupRequest(const QString &name, quint16 group_id)
{
	// Testing the name and group_id
	QMapIterator<quint16, QString> itr(m_groups);
	while (itr.hasNext()) {
		itr.next();
		if (itr.value().compare(name, Qt::CaseInsensitive) == 0) {
			qWarning() << QString("The group name \"%1\" already exists").arg(name);
			return 0;
		}

		if (group_id != 0 && itr.key() == group_id) {
			qWarning() << QString("The group with id \"%1\" already exists").arg(group_id);
			return 0;
		}
	}

	// Adding the new group in the server contactlist
	SSIItem item;
	item.item_type = SsiGroup;
	item.record_name = name;
	if (group_id == 0) {
		group_id = rand() % 0x03e6;
		while (m_groups.contains(0x03e6))
			group_id = rand() % 0x03e6;
	}
	item.group_id = group_id;
	item.tlvs.insert(0x00c8);

	sendCLModifyStart();
	sendCLOperator(item, ListsAddToList);
	sendCLModifyEnd();
	return group_id;
}

void Roster::sendRemoveGroupRequest(quint16 id)
{
	if (m_groups.contains(id)) {
		SSIItem item;
		item.item_type = SsiGroup;
		item.group_id = id;

		sendCLModifyStart();
		sendCLOperator(item, ListsRemoveFromList);
		sendCLModifyEnd();
	} else
		debug() << QString("The group (%1) does not exist").arg(id);
}

IcqContact *Roster::sendAddContactRequest(const QString &contact_id, const QString &contact_name, quint16 group_id)
{
	SSIItem item;
	item.item_type = SsiBuddy;
	item.record_name = contact_id;
	item.group_id = group_id;
	item.item_id = getItemId();
	item.tlvs.insert(SsiBuddyNick, contact_name);
	item.tlvs.insert(SsiBuddyReqAuth);

	sendCLModifyStart();
	sendCLOperator(item, ListsAddToList);
	sendCLModifyEnd();

	IcqContact *contact = new IcqContact(contact_id, m_account);
	m_not_in_list.insert(contact_id, contact);
	emit m_account->contactCreated(contact);
	return contact;

}

void Roster::sendRemoveContactRequst(const QString &contact_id)
{
	IcqContact *contact = m_contacts.value(contact_id);
	if (contact) {
		SSIItem item;
		item.item_type = SsiBuddy;
		item.item_id = contact->d_func()->user_id;
		item.group_id = contact->d_func()->group_id;
		item.record_name = contact->id();

		sendCLModifyStart();
		sendCLOperator(item, ListsRemoveFromList);
		sendCLModifyEnd();
	} else
		debug() << QString("The contact (%1) does not exist").arg(contact_id);
}

void Roster::sendRenameContactRequest(const QString &contact_id, const QString &name)
{
	IcqContact *contact = m_contacts.value(contact_id);
	if (contact) {
		SSIItem item;
		item.item_type = SsiBuddy;
		item.item_id = contact->d_func()->user_id;
		item.group_id = contact->d_func()->group_id;
		item.record_name = contact->id();
		item.tlvs.insert(SsiBuddyNick, name);
		if (!contact->property("authorized").toBool())
			item.tlvs.insert(SsiBuddyReqAuth);
		QString comment = contact->property("comment").toString();
		if (!comment.isEmpty())
			item.tlvs.insert(SsiBuddyComment, comment);

		sendCLModifyStart();
		sendCLOperator(item, ListsUpdateGroup);
		sendCLModifyEnd();
	} else
		debug() << QString("The contact (%1) does not exist").arg(contact_id);
}

void Roster::sendRenameGroupRequest(quint16 group_id, const QString &name)
{
	if (m_groups.contains(group_id)) {
		SSIItem item;
		item.item_type = SsiGroup;
		item.group_id = group_id;
		item.record_name = name;

		sendCLModifyStart();
		sendCLOperator(item, ListsUpdateGroup);
		sendCLModifyEnd();
	} else
		debug() << QString("The group (%1) does not exist").arg(group_id);
}

void Roster::setVisibility(Visibility visibility)
{
	SSIItem item;
	item.item_type = SsiVisibility;
	item.item_id = m_visibility_id;
	TLV data(0x00CA);
	data.appendValue<quint8>(visibility);
	item.tlvs.insert(data);
	item.tlvs.insert<qint32>(0x00C9, 0xffffffff);
	sendCLModifyStart();
	if (m_visibility_id == 0) {
		item.item_id = getItemId(1);
		sendCLOperator(item, ListsAddToList);
		m_visibility_id = item.item_id;
	} else
		sendCLOperator(item, ListsUpdateGroup);
	sendCLModifyEnd();
	m_visibility = visibility;
}

void Roster::handleServerCListReply(const SNAC &sn)
{
	if (!(sn.flags() & 0x0001))
		m_state = RosterReceived;
	quint8 version = sn.readSimple<quint8>();
	quint16 count = sn.readSimple<quint16>();
	bool is_last = !(sn.flags() & 0x0001);
	debug() << "SSI: number of entries is" << count << "version is" << version;
	for (uint i = 0; i < count; i++) {
		SSIItem item(sn);
		handleSSIItem(item, mt_add_modify);
	}
	debug() << "is_last" << is_last;
	if (is_last) {
		quint32 last_info_update = sn.readSimple<quint32>();
		debug() << "SrvLastUpdate" << last_info_update;
		m_conn->setProperty("SrvLastUpdate", last_info_update);
		sendRosterAck();
		m_conn->finishLogin();

		if (!m_groups.contains(not_in_list_group)) {
			const QString not_in_list_str = tr("Not In List");
			QString group_name(not_in_list_str);
			for (int i = 1;; ++i) {
				bool found = false;
				foreach(const QString &grp, m_groups)
				{
					if (grp.compare(group_name, Qt::CaseInsensitive) == 0) {
						found = true;
						break;
					}
				}
				if (!found)
					break;
				group_name = QString("%1 %2").arg(not_in_list_str).arg(i);
			}
			sendAddGroupRequest(group_name, not_in_list_group);
		}
	}
}

void Roster::handleSSIItem(const SSIItem &item, ModifingType type)
{
	if (type == mt_remove)
		handleRemoveCLItem(item);
	else
		handleAddModifyCLItem(item, type);
	m_ssi_id_set.insert(item.item_id);
}

void Roster::handleAddModifyCLItem(const SSIItem &item, ModifingType type)
{
	Q_ASSERT(type != mt_remove);
	switch (item.item_type) {
	case SsiBuddy: {
		IcqContact *contact = m_contacts.value(item.record_name);
		// record name contains uin
		bool is_adding = !contact;
		if (is_adding && type == mt_modify) {
			debug() << "The contact does not exist" << item.record_name;
			return;
		}
		if (!is_adding && type == mt_add) {
			debug() << "The contact already is in contactlist";
			return;
		}
		if (is_adding) {
			contact = m_not_in_list.take(item.record_name);
			if (!contact) {
				contact = new IcqContact(item.record_name, m_account);
				emit m_account->contactCreated(contact);
			}
			m_contacts.insert(item.record_name, contact);
			contact->d_func()->group_id = item.group_id;
			if (item.tlvs.contains(SsiBuddyNick))
				contact->d_func()->name = item.tlvs.value<QString>(SsiBuddyNick);
			if (item.tlvs.contains(SsiBuddyComment))
				contact->setProperty("comment", item.tlvs.value<QString>(SsiBuddyComment));
			bool auth = !item.tlvs.contains(SsiBuddyReqAuth);
			contact->setProperty("authorized", auth);
			if (ContactList::instance())
				ContactList::instance()->addContact(contact);
			debug() << "The contact is added" << contact->id() << contact->name() << item.item_id;
		} else {
			// name
			QString new_name = item.tlvs.value<QString>(SsiBuddyNick);
			if (!new_name.isEmpty() && new_name != contact->d_func()->name) {
				contact->d_func()->name = new_name;
				emit contact->nameChanged(new_name);
			}
			// comment
			QString new_comment = item.tlvs.value<QString>(SsiBuddyComment);
			if (!new_comment.isEmpty() && new_comment != contact->property("comment").toString()) {
				contact->setProperty("comment", new_comment);
				// TODO: emit ...
			}
			// auth
			bool new_auth = !item.tlvs.contains(SsiBuddyReqAuth);
			contact->setProperty("authorized", new_auth);
			// TODO: emit ...
			debug() << "The contact is updated" << contact->id() << contact->name() << item.item_id;
		}
		contact->d_func()->user_id = item.item_id;
		break;
	}
	case SsiGroup:
		if (item.group_id > 0) {
			// record name contains name of group
			QMap<quint16, QString>::iterator itr = m_groups.find(item.group_id);
			if (itr == m_groups.end()) {
				if (type != mt_modify) {
					m_groups.insert(item.group_id, item.record_name);
					debug() << "The group is added" << item.group_id << item.record_name;
				} else
					debug() << "The group does not exist" << item.group_id << item.record_name;
			} else {
				if (type != mt_add) {
					itr.value() = item.record_name;
					debug() << "The group is updated" << item.group_id << item.record_name;
				} else
					debug() << "The group already is in contactlist" << item.group_id << item.record_name;
			}
		} else {
		}
		break;
	case SsiVisibility:
		m_visibility_id = item.item_id;
		m_visibility = static_cast<Visibility>(item.tlvs.value<quint8>(0x00CA, AllowAllUsers));
		debug() << "Visibility" << m_visibility_id << m_visibility;
		break;
	case SsiBuddyIcon:
		if (m_account->avatarsSupport() && item.tlvs.contains(0x00d5)) {
			DataUnit data(item.tlvs.value(0x00d5));
			quint8 flags = data.readSimple<quint8>();
			QByteArray hash = data.readData<quint8>();
			if (hash.size() == 16)
				m_conn->buddyPictureService()->sendUpdatePicture(m_account, 1, flags, hash);
		}
		break;
	default:
		debug(Verbose) << "Dump of unknown SSI item:" << item.toString();
	}
}

void Roster::handleRemoveCLItem(const SSIItem &item)
{
	switch (item.item_type) {
	case SsiBuddy: {
		QHash<QString, IcqContact *>::iterator contact_itr = m_contacts.begin();
		QHash<QString, IcqContact *>::iterator end_itr = m_contacts.end();
		while (contact_itr != end_itr) {
			if (contact_itr.value()->d_func()->user_id == item.item_id)
				break;
			++contact_itr;
		}
		if (contact_itr == end_itr) {
			debug() << "The contact does not exist" << item.item_id << item.record_name;
			break;
		}
		removeContact(*contact_itr);
		m_contacts.erase(contact_itr);
		debug() << "The contact is removed" << item.item_id << item.record_name;
		break;
	}
	case SsiGroup: {
		QMap<quint16, QString>::iterator group_itr = m_groups.find(item.group_id);
		if (group_itr == m_groups.end()) {
			debug() << "The group does not exist" << item.group_id;
			break;
		}
		// Removing all contacts in the group.
		QHash<QString, IcqContact *>::iterator contact_itr = m_contacts.begin();
		QHash<QString, IcqContact *>::iterator contact_end_itr = m_contacts.end();
		while (contact_itr != contact_end_itr) {
			if (contact_itr.value()->d_func()->group_id == item.group_id) {
				removeContact(*contact_itr);
				contact_itr = m_contacts.erase(contact_itr);
			} else
				++contact_itr;
		}
		// Removing the group.
		m_groups.erase(group_itr);
		debug() << "The group is removed" << item.group_id;
		break;
	}
	default:
		debug(Verbose) << "Dump of unknown SSI item:" << item.toString();
	}

}

void Roster::removeContact(IcqContact *contact)
{
	if (ContactList::instance())
		ContactList::instance()->removeContact(contact);
	delete contact;
}

void Roster::handleSSIServerAck(const SNAC &sn)
{
	sn.skipData(8); // cookie?
	while (sn.dataSize() != 0) {
		quint16 error = sn.readSimple<quint16>();
		if (error == 0) {
			SSIHistoryItem operation = m_ssi_history.dequeue();
			debug() << "The last SSI operation is successfully done" << operation.type << operation.item.item_type << operation.item.record_name << operation.item.item_id << operation.item.group_id;
			handleSSIItem(operation.item, operation.type);
			break;
		}
		QString error_str;
		if (error == 0x0002)
			error_str = "Item you want to modify not found in list";
		else if (error == 0x0003)
			error_str = "Item you want to add allready exists";
		else if (error == 0x000a)
			error_str = "Error adding item (invalid id, allready in list, invalid data)";
		else if (error == 0x000c)
			error_str = "Can't add item. Limit for this type of items exceeded";
		else if (error == 0x000d)
			error_str = "Trying to add ICQ contact to an AIM list";
		else if (error == 0x000e)
			error_str = "Can't add this contact because it requires authorization";
		else
			error_str = QString::number(error);
		debug() << "SSI operation error" << error_str;
	}
}

void Roster::handleUserOnline(const SNAC &snac)
{
	QString uin = snac.readData<quint8>();
	IcqContact *contact = m_contacts.value(uin, 0);
	// We don't know this contact
	if (!contact)
		return;
	quint16 warning_level = snac.readSimple<quint16>();
	TLVMap tlvs = snac.readTLVChain<quint16>();

	// status.
	Status oldStatus = contact->status();
	quint16 statusFlags = 0;
	quint16 status = 0;
	if (tlvs.contains(0x06)) {
		DataUnit status_data(tlvs.value(0x06));
		statusFlags = status_data.readSimple<quint16>();
		status = status_data.readSimple<quint16>();
	}
	contact->setStatus(icqStatusToQutim(status));
	debug() << contact->name() << "changed status to " << contact->status();

	// Status note
	SessionDataItemMap status_note_data(tlvs);
	if (status_note_data.contains(0x0d)) {
		DataUnit data(status_note_data.value(0x0d).data);
		quint16 time = data.readSimple<quint16>();
		debug() << "Status note update time" << time;
	}
	if (status_note_data.contains(0x02)) {
		DataUnit data(status_note_data.value(0x02).data);
		QByteArray note_data = data.readData<quint16>();
		QByteArray encoding = data.readData<quint16>();
		QTextCodec *codec;
		if (encoding.isEmpty())
			codec = defaultCodec();
		else
			codec = QTextCodec::codecForName(encoding);
		if (!codec) {
			debug() << "Server sent wrong encoding for status note";
			codec = defaultCodec();
		}
		contact->setProperty("statusText", codec->toUnicode(note_data));
	}

	// XStatus
	Capabilities newCaps;
	DataUnit data(tlvs.value(0x000d));
	while (data.dataSize() >= 16) {
		Capability capability(data.readData(16));
		newCaps << capability;
	}
	qint8 moodIndex = -1;
	if (status_note_data.contains(0x0e)) {
		QString moodStr = QString::fromAscii(status_note_data.value(0x0e).data);
		if (moodStr.startsWith("icqmood")) {
			bool ok;
			moodIndex = moodStr.mid(7, -1).toInt(&ok);
			if (!ok)
				moodIndex = -1;
		}
	}
	if (Xtraz::handelXStatusCapabilities(contact, newCaps, moodIndex)) {
		QString notify = QString("<srv><id>cAwaySrv</id><req><id>AwayStat</id>"
			"<trans>1</trans><senderId>%1</senderId></req></srv>"). arg(m_account->id());
		XtrazRequest xstatusRequest(contact, "<Q><PluginID>srvMng</PluginID></Q>", notify);
		m_conn->send(xstatusRequest);
	}

	if (oldStatus != Offline)
		return;

	if (tlvs.contains(0x000c)) { // direct connection info
		DataUnit data(tlvs.value(0x000c));
		DirectConnectionInfo info =
		{
				QHostAddress(data.readSimple<quint32>()),
				QHostAddress(),
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
		contact->d_func()->dc_info = info;
	}

	if (m_account->avatarsSupport() && tlvs.contains(0x001d)) { // avatar
		DataUnit data(tlvs.value(0x001d));
		quint16 id = data.readSimple<quint16>();
		quint8 flags = data.readSimple<quint8>();
		QByteArray hash = data.readData<quint8>();
		if (hash.size() == 16)
			m_conn->buddyPictureService()->sendUpdatePicture(contact, id, flags, hash);
	}

	// Updating capabilities
	if (tlvs.contains(0x0019)) {
		DataUnit data(tlvs.value(0x0019));
		while (data.dataSize() >= 2)
			newCaps.push_back(Capability(data.readData(2)));
	}
	contact->setCapabilities(newCaps);

	ClientIdentify identify;
	identify.identify(contact);
	debug() << contact->name() << "uses" << contact->property("client_id").toString();
}

void Roster::handleUserOffline(const SNAC &snac)
{
	QString uin = snac.readString<quint8>();
	IcqContact *contact = m_contacts.value(uin, 0);
	// We don't know this contact
	if (!contact)
		return;
	contact->setStatus(Offline);
	//	quint16 warning_level = snac.readSimple<quint16>();
	//	TLVMap tlvs = snac.readTLVChain<quint16>();
	//	tlvs.value(0x0001); // User class
}

void Roster::sendRosterAck()
{
	SNAC snac(ListsFamily, ListsGotList);
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
	m_ssi_history.enqueue(SSIHistoryItem(item, (ModifingType) operation));
	SNAC snac(ListsFamily, operation);
	snac.appendData<quint16>(item.record_name);
	snac.appendSimple<quint16>(item.group_id);
	snac.appendSimple<quint16>(item.item_id);
	snac.appendSimple<quint16>(item.item_type);
	snac.appendSimple<quint16>(item.tlvs.valuesSize());
	snac.appendData(item.tlvs);
	m_conn->send(snac);
}

quint16 Roster::getItemId(quint16 value)
{
	quint16 id = (value == 0) ? (rand() % 0x03e6) : value;
	forever {
		if (m_ssi_id_set.contains(id))
			id = rand() % 0x03e6;
		else
			break;
	}
}

} // namespace Icq

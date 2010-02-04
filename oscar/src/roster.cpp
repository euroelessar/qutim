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
#include "feedbag.h"
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

class SsiHandler : public FeedbagItemHandler
{
public:
	explicit SsiHandler(IcqAccount *account, QObject *parent = 0):
		FeedbagItemHandler(parent), m_account(account), m_visibilityId(1)
	{
		m_types << SsiBuddy << SsiGroup << SsiVisibility << SsiBuddyIcon;
	}
	void handleFeedbagItem(Feedbag *feedbag, const FeedbagItem &item, Feedbag::ModifyType type, FeedbagError error);
	void handleAddModifyCLItem(const FeedbagItem &item, Feedbag::ModifyType type);
	void handleRemoveCLItem(const FeedbagItem &item);
	void removeContact(IcqContact *contact);
	IcqContact *contact(const QString &id) { return m_contacts.value(id); }
	QMap<quint16, QString> m_groups;
	QHash<QString, IcqContact *> m_contacts;
	QHash<QString, IcqContact *> m_notInList;
	IcqAccount *m_account;
	quint16 m_visibilityId;
	Visibility m_visibility;
};

void SsiHandler::handleFeedbagItem(Feedbag *feedbag, const FeedbagItem &item, Feedbag::ModifyType type, FeedbagError error)
{
	if (error != FeedbagError::NoError)
		return;
	if (type == Feedbag::Remove)
		handleRemoveCLItem(item);
	else
		handleAddModifyCLItem(item, type);
}

void SsiHandler::handleAddModifyCLItem(const FeedbagItem &item, Feedbag::ModifyType type)
{
	switch (item.type()) {
	case SsiBuddy: {
		IcqContact *contact = m_contacts.value(item.name());
		IcqContactPrivate *d;
		// record name contains uin
		bool is_adding = !contact;
		if (is_adding && type == Feedbag::Modify) {
			debug() << "The contact does not exist" << item.name();
			return;
		}
		if (!is_adding && type == Feedbag::Add) {
			debug() << "The contact already presents in contactlist";
			return;
		}
		if (is_adding) {
			contact = m_notInList.take(item.name());
			if (!contact) {
				contact = new IcqContact(item.name(), m_account);
				emit m_account->contactCreated(contact);
			}
			d = contact->d_func();
			m_contacts.insert(item.name(), contact);
			d->group_id = item.groupId();
			if (item.containsField(SsiBuddyNick))
				d->name = item.field<QString>(SsiBuddyNick);
			if (item.containsField(SsiBuddyComment))
				contact->setProperty("comment", item.field<QString>(SsiBuddyComment));
			bool auth = !item.containsField(SsiBuddyReqAuth);
			contact->setProperty("authorized", auth);
			if (ContactList::instance())
				ContactList::instance()->addContact(contact);
			debug() << "The contact is added" << contact->id() << contact->name() << item.itemId();
		} else {
			d = contact->d_func();
			// name
			QString new_name = item.field<QString>(SsiBuddyNick);
			if (!new_name.isEmpty() && new_name != contact->d_func()->name) {
				contact->d_func()->name = new_name;
				emit contact->nameChanged(new_name);
			}
			// comment
			QString new_comment = item.field<QString>(SsiBuddyComment);
			if (!new_comment.isEmpty() && new_comment != contact->property("comment").toString()) {
				contact->setProperty("comment", new_comment);
				// TODO: emit ...
			}
			// auth
			bool new_auth = !item.containsField(SsiBuddyReqAuth);
			contact->setProperty("authorized", new_auth);
			// TODO: emit ...
			debug() << "The contact is updated" << contact->id() << contact->name() << item.itemId();
		}
		d->user_id = item.itemId();
		break;
	}
	case SsiGroup:
		if (item.groupId() > 0) {
			// record name contains name of group
			QMap<quint16, QString>::iterator itr = m_groups.find(item.groupId());
			if (itr == m_groups.end()) {
				if (type != Feedbag::Modify) {
					m_groups.insert(item.groupId(), item.name());
					debug() << "The group is added" << item.name() << item.groupId();
				} else
					debug() << "The group does not exist" << item.name() << item.groupId();
			} else {
				if (type != Feedbag::Add) {
					itr.value() = item.name();
					debug() << "The group is updated" << item.name() << item.groupId();
				} else
					debug() << "The group already is in contactlist" << item.name() << item.groupId();
			}
		} else {
		}
		break;
	case SsiVisibility:
		m_visibilityId = item.itemId();
		m_visibility = static_cast<Visibility>(item.field<quint8>(0x00CA, AllowAllUsers));
		debug() << "Visibility" << m_visibilityId << m_visibility;
		break;
	case SsiBuddyIcon:
		if (m_account->avatarsSupport() && item.containsField(0x00d5)) {
			DataUnit data(item.field(0x00d5));
			quint8 flags = data.readSimple<quint8>();
			QByteArray hash = data.readData<quint8>();
			if (hash.size() == 16)
				m_account->connection()->buddyPictureService()->sendUpdatePicture(m_account, 1, flags, hash);
		}
		break;
	}
}

void SsiHandler::handleRemoveCLItem(const FeedbagItem &item)
{
	switch (item.type()) {
	case SsiBuddy: {
		IcqContact *contact = m_contacts.take(item.name());
		if (!contact) {
			debug() << "The contact does not exist" << item.itemId() << item.name();
			break;
		}
		removeContact(contact);
		debug() << "The contact is removed" << item.itemId() << item.name();
		break;
	}
	case SsiGroup: {
		QString group = m_groups.take(item.groupId());
		if (group.isNull()) {
			debug() << "The group does not exist" << item.groupId();
			break;
		}
		// Removing all contacts in the group.
		QHash<QString, IcqContact *>::iterator contactItr = m_contacts.begin();
		QHash<QString, IcqContact *>::iterator contactEntItr = m_contacts.end();
		while (contactItr != contactEntItr) {
			if (contactItr.value()->d_func()->group_id == item.groupId()) {
				removeContact(*contactItr);
				contactItr = m_contacts.erase(contactItr);
			} else
				++contactItr;
		}
		debug() << "The group is removed" << item.groupId();
		break;
	}
	}
}

void SsiHandler::removeContact(IcqContact *contact)
{
	if (ContactList::instance())
		ContactList::instance()->removeContact(contact);
	delete contact;
}

Roster::Roster(IcqAccount *account)
{
	connect(account, SIGNAL(loginFinished()), SLOT(loginFinished()));
	m_account = account;
	m_ssiHandler = new SsiHandler(m_account, this);
	m_conn = account->connection();
	m_conn->feedbag()->registerHandler(m_ssiHandler);
	m_infos << SNACInfo(ListsFamily, ListsError)
			<< SNACInfo(ListsFamily, ListsAuthRequest)
			<< SNACInfo(ListsFamily, ListsSrvAuthResponse)
			<< SNACInfo(BuddyFamily, UserOnline)
			<< SNACInfo(BuddyFamily, UserOffline)
			<< SNACInfo(BuddyFamily, UserSrvReplyBuddy)
			<< SNACInfo(ExtensionsFamily, ExtensionsMetaError);
}

void Roster::handleSNAC(AbstractConnection *c, const SNAC &sn)
{
	Q_ASSERT(c == m_conn);
	switch ((sn.family() << 16) | sn.subtype()) {
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

QString Roster::groupId2Name(quint16 id)
{
	return m_ssiHandler->m_groups.value(id);
}

IcqContact *Roster::contact(const QString &uin)
{
	return m_ssiHandler->m_contacts.value(uin);
}

const QHash<QString, IcqContact *> &Roster::contacts() const
{
	return m_ssiHandler->m_contacts;
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

quint16 Roster::sendAddGroupRequest(const QString &name)
{
	FeedbagItem item = m_conn->feedbag()->group(name, Feedbag::GenerateId);
	if (item.isInList()) {
		debug() << QString("The group named \"%1\" already exists").arg(name);
	} else {
		item.setField(0x00c8); // ???
		item.update();
	}
	return item.groupId();
}

void Roster::sendRemoveGroupRequest(const QString &name)
{
	FeedbagItem item = m_conn->feedbag()->group(name);
	if (item.isNull())
		debug() << QString("The group named \"%1\" does not exist").arg(name);
	else
		item.remove();
}

IcqContact *Roster::sendAddContactRequest(const QString &contactId, const QString &contactName, quint16 groupId)
{
	IcqContact *contact = m_ssiHandler->contact(contactId);
	if (!contact) {
		FeedbagItem item = m_conn->feedbag()->item(SsiBuddy, contactId, Feedbag::GenerateId | Feedbag::DontLoadLocal);
		item.setGroup(groupId);
		item.setField<QString>(SsiBuddyNick, contactName);
		item.setField(SsiBuddyReqAuth);
		item.update();
		contact = new IcqContact(contactId, m_account);
		m_ssiHandler->m_notInList.insert(contactId, contact);
		emit m_account->contactCreated(contact);
	} else {
		debug() << contactId << "already contains in your contact list";
	}
	return contact;

}

void Roster::sendRemoveContactRequst(const QString &contactId)
{
	if (!m_conn->feedbag()->removeItem(SsiBuddy, contactId)) {
		debug() << QString("The contact (%1) does not contain in your contact list").arg(contactId);
	}
}

void Roster::sendRenameContactRequest(const QString &contactId, const QString &name)
{
	FeedbagItem item = m_conn->feedbag()->item(SsiBuddy, contactId);
	if (item.isInList()) {
		item.setField(SsiBuddyNick, name);
		item.update();
	} else {
		debug() << QString("The contact (%1) does not contain in your contact list").arg(contactId);
	}
}

void Roster::sendRenameGroupRequest(quint16 groupId, const QString &name)
{
	FeedbagItem item = m_conn->feedbag()->group(groupId);
	if (item.isInList()) {
		item.setName(name);
		item.update();
	} else {
		debug() << QString("The group (%1) does not exist").arg(groupId);
	}
}

void Roster::setVisibility(Visibility visibility)
{
	FeedbagItem item = m_conn->feedbag()->item(SsiVisibility, m_ssiHandler->m_visibilityId, Feedbag::GenerateId);
	TLV data(0x00CA);
	data.appendValue<quint8>(visibility);
	item.setField(data);
	item.setField<qint32>(0x00C9, 0xffffffff);
	item.update();
}

void Roster::loginFinished()
{
	const QString not_in_list_str = QT_TRANSLATE_NOOP("ContactList", "Not In List");
	FeedbagItem item = m_conn->feedbag()->group(not_in_list_group, Feedbag::GenerateId);
	if (!item.isInList()) {
		QString groupName(not_in_list_str);
		for (int i = 1;; ++i) {
			if (m_conn->feedbag()->testGroupName(groupName))
				item.setName(groupName);
			groupName = QString("%1 %2").arg(not_in_list_str).arg(i);
		}
		item.update();
	}
}

void Roster::handleUserOnline(const SNAC &snac)
{
	QString uin = snac.readData<quint8>();
	IcqContact *contact = m_ssiHandler->m_contacts.value(uin, 0);
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
	IcqContact *contact = m_ssiHandler->m_contacts.value(uin, 0);
	// We don't know this contact
	if (!contact)
		return;
	contact->setStatus(Offline);
	//	quint16 warning_level = snac.readSimple<quint16>();
	//	TLVMap tlvs = snac.readTLVChain<quint16>();
	//	tlvs.value(0x0001); // User class
}

} // namespace Icq

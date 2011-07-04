/****************************************************************************
 *  roster.cpp
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

#include "oscarroster_p.h"
#include "icqcontact_p.h"
#include "icqaccount_p.h"
#include "icqprotocol.h"
#include "oscarconnection.h"
#include "buddycaps.h"
#include "messages.h"
#include "xtraz.h"
#include "feedbag.h"
#include "sessiondataitem.h"
#include <qutim/messagesession.h>
#include <qutim/notification.h>

namespace qutim_sdk_0_3 {

namespace oscar {

using namespace Util;

Roster::Roster()
{
	foreach (Account *account, IcqProtocol::instance()->accounts())
		accountAdded(account);
	connect(IcqProtocol::instance(),
			SIGNAL(accountCreated(qutim_sdk_0_3::Account*)),
			SLOT(accountAdded(qutim_sdk_0_3::Account*)));
	m_infos << SNACInfo(ServiceFamily, ServiceServerAsksServices)
			<< SNACInfo(BuddyFamily, UserOnline)
			<< SNACInfo(BuddyFamily, UserOffline)
			<< SNACInfo(BuddyFamily, UserSrvReplyBuddy);
	m_types << SsiBuddy << SsiGroup << SsiTags;
}

bool Roster::handleFeedbagItem(Feedbag *feedbag, const FeedbagItem &item, Feedbag::ModifyType type, FeedbagError error)
{
	if (error != FeedbagError::NoError)
		return false;
	if (type == Feedbag::Remove)
		handleRemoveCLItem(feedbag->account(), item);
	else
		handleAddModifyCLItem(feedbag->account(), item);
	return true;
}

void Roster::handleAddModifyCLItem(IcqAccount *account, const FeedbagItem &item)
{
	switch (item.type()) {
	case SsiBuddy: {
		if (item.name().isEmpty())
			break;
		ConnectingInfo *connInfo = account->d_func()->connectingInfo.data();
		IcqContact *contact = account->getContact(item.name());
		if (!contact) {
			contact = account->getContact(item.name(), true, true);
			if (connInfo)
				connInfo->createdContacts << contact;
		}
		IcqContactPrivate *d = contact->d_func();
		// There is no need to remove the contact after connecting.
		if (connInfo)
			connInfo->removedContacts.remove(contact->id());
		QList<FeedbagItem>::iterator itr = d->items.begin();
		QList<FeedbagItem>::iterator endItr = d->items.end();
		bool newTag = true;
		while (itr != endItr) {
			if (itr->itemId() == item.itemId() && itr->groupId() == item.groupId()) {
				*itr = item;
				newTag = false;
				break;
			}
			++itr;
		}
		bool added = false;
		if (newTag) {
			QStringList previous;
			if (!connInfo)
				previous = contact->tags();
			if (d->items.isEmpty()) {
				// Now, the contact should not be removed after destroying its session
				// as it was added to the server contact list.
				ChatSession *session = ChatLayer::instance()->get(contact, false);
				if (session)
					disconnect(session, SIGNAL(destroyed()), contact, SLOT(deleteLater()));
				// Add the contact to the contact list.
				loadTagsFromFeedbag(contact);
//				debug().nospace() << "The contact " << contact->id() << " (" << contact->name() << ") has been added";
				if (!connInfo)
					emit contact->inListChanged(true);
				added = true;
			}
			d->items << item;
			if (!connInfo)
				emit contact->tagsChanged(contact->tags(), previous);
		}
		if (!added)
			debug().nospace() << "The contact " << contact->id() << " (" << contact->name() << ") has been updated";
		// proto
		QString new_proto = item.field<QString>(SsiBuddyProto);
		if (!new_proto.isEmpty() && new_proto != contact->d_func()->proto) {
			contact->d_func()->proto = new_proto;
		}
		// name
		QString new_name = item.field<QString>(SsiBuddyNick);
		if (!new_name.isEmpty() && new_name != contact->d_func()->name) {
			QString previous = contact->d_func()->name;
			contact->d_func()->name = new_name;
			emit contact->nameChanged(new_name, previous);
		}
		// comment
		QString new_comment = item.field<QString>(SsiBuddyComment);
		if (!new_comment.isEmpty() && new_comment != contact->property("comment").toString()) {
			contact->setProperty("comment", new_comment);
			// TODO: emit ...
		}
		break;
	}
	case SsiGroup: {
		FeedbagItem old = account->feedbag()->groupItem(item.groupId());
		if (old.isInList() && old.name() != item.name()) {
			foreach (const FeedbagItem &i, account->feedbag()->group(item.groupId())) {
				QSet<QString> groups;
				IcqContact *contact = account->getContact(i.name());
				QStringList previous = contact->tags();
				foreach (const FeedbagItem &i, contact->d_func()->items) {
					if (item.groupId() == i.groupId())
						groups << item.name();
					else
						groups << i.name();
				}
				emit contact->tagsChanged(contact->tags(), previous);
			}
			debug(Verbose) << "The group" << old.name() << "has been renamed to" << item.name();
		} else {
			debug(Verbose) << "The group" << item.name() << "has been added";
		}
		break;
	}
	case SsiTags: {
		IcqContact *contact = account->getContact(item.name());
		if (contact) {
			QStringList previous = contact->tags();
			contact->d_func()->tags = readTags(item);
			if (!account->d_func()->connectingInfo)
				emit contact->tagsChanged(contact->tags(), previous);
		}
		break;
	}
	}
}

void Roster::handleRemoveCLItem(IcqAccount *account, const FeedbagItem &item)
{
	switch (item.type()) {
	case SsiBuddy: {
		IcqContact *contact = account->getContact(item.name());
		if (!contact) {
			warning() << "The contact" << item.name() << "does not exist";
			break;
		}
		removeContactFromGroup(contact, item.groupId());
		break;
	}
	case SsiGroup: {
		foreach (IcqContact *contact, account->contacts())
			removeContactFromGroup(contact, item.groupId());
		debug() << "The group" << item.name() << "has been removed";
		break;
	}
	case SsiTags: {
		IcqContact *contact = account->getContact(item.name());
		if (contact) {
			QStringList previous = contact->tags();
			contact->d_func()->tags.clear();
			emit contact->tagsChanged(contact->tags(), previous);
		}
		break;
	}
	}
}

void Roster::removeContactFromGroup(IcqContact *contact, quint16 groupId)
{
	QStringList previous = contact->tags();
	QList<FeedbagItem> &items = contact->d_func()->items;
	QList<FeedbagItem>::iterator itr = items.begin();
	QList<FeedbagItem>::iterator endItr = items.end();
	bool found = false;
	while (itr != endItr) {
		if (itr->groupId() == groupId) {
			items.erase(itr);
			found = true;
			break;
		}
		++itr;
	}
	if (found) {
		if (items.isEmpty()) {
//			debug().nospace() << "The contact " << contact->id()
//					<< " (" << contact->name() << ") has been removed";
			removeContact(contact);
		} else {
//			debug().nospace() << "The contact " << contact->id() << " ("
//					<< contact->name() << ") has been removed from "
//					<< contact->account()->feedbag()->groupItem(groupId).name();
			emit contact->tagsChanged(contact->tags(), previous);
		}
	}
}

void Roster::loadTagsFromFeedbag(IcqContact *contact)
{
	FeedbagItem tagsItem = contact->account()->feedbag()->item(SsiTags, contact->id(), 0);
	if (tagsItem.isInList())
		contact->d_func()->tags = readTags(tagsItem);
}

void Roster::removeContact(IcqContact *contact)
{
	emit contact->inListChanged(false);
	// Remove tags.
	FeedbagItem item = contact->account()->feedbag()->item(SsiTags, contact->id(), 0);
	if (!item.isNull())
		item.remove();
	ChatSession *session = ChatLayer::instance()->get(contact, false);
	if (session)
		// The contact has been removed, but its session still exists,
		// thus we have to wait until the session is destroyed.
		connect(session, SIGNAL(destroyed()), contact, SLOT(deleteLater()));
	else
		// Well... There is no need in this contact.
		contact->deleteLater();
}

QStringList Roster::readTags(const FeedbagItem &item)
{
	QStringList newTags;
	DataUnit newTagsData = item.field(SsiBuddyTags);
	while (newTagsData.dataSize() > 2) {
		QString data = newTagsData.read<QString, quint16>();
		if (!data.isEmpty())
			newTags << data;
	}
	return newTags;
}

void Roster::handleSNAC(AbstractConnection *conn, const SNAC &sn)
{
	switch ((sn.family() << 16) | sn.subtype()) {
	case ServiceFamily << 16 | ServiceServerAsksServices: {
		// Requesting client-side contactlist rights
		SNAC snac(BuddyFamily, UserCliReqBuddy);
		// Unknown rights from ICQ 7.5
//		snac.appendTLV<quint16>(0x0005, 0x0f);
//		snac.appendTLV(0x0006, QByteArray::fromHex("000000"));
//		snac.appendTLV<quint8>(0x0007, 0x00);
		// Request facebook contacts
		snac.appendTLV<quint8>(0x0008, 1);
		// Query flags: 1 = Enable Avatars
		//              2 = Enable offline status message notification
		//              4 = Enable Avatars for offline contacts
		//              8 = Use reject for not authorized contacts
		//snac.appendTLV<quint16>(0x05, 7);
		conn->send(snac);
		break;
	}
	case BuddyFamily << 16 | UserOnline:
		handleUserOnline(conn->account(), sn);
		break;
	case BuddyFamily << 16 | UserOffline:
		handleUserOffline(conn->account(), sn);
		break;
	case BuddyFamily << 16 | UserSrvReplyBuddy:
		debug() << IMPLEMENT_ME << "BuddyFamily, UserSrvReplyBuddy";
		break;
	}
}

void Roster::handleUserOnline(IcqAccount *account, const SNAC &snac)
{
	QString uin = snac.read<QString, quint8>();
	IcqContact *contact = account->getContact(uin);
	// We don't know this contact
	if (!contact)
		return;
	quint16 warning_level = snac.read<quint16>();
	Q_UNUSED(warning_level);
	TLVMap tlvs = snac.read<TLVMap, quint16>();
	// status.
	Status oldStatus = contact->status();
	quint16 statusId = 0;
	quint16 statusFlags = 0;
	OscarStatus status = contact->status();
	if (tlvs.contains(0x06)) {
		DataUnit status_data(tlvs.value(0x06));
		statusFlags = status_data.read<quint16>();
		statusId = status_data.read<quint16>();
		if (!status.setStatusFlag(statusId & 0x0fff))
			status.setStatusType(Status::Online);
	} else {
		status.setStatusType(Status::Online);
	}
	// Status note
	SessionDataItemMap statusNoteData(tlvs.value(0x1D));
	if (statusNoteData.contains(0x0d)) {
		quint16 time = statusNoteData.value(0x0d).read<quint16>();
		debug() << "Status note update time" << time;
	}
	if (statusNoteData.contains(0x02)) {
		DataUnit data(statusNoteData.value(0x02));
		QByteArray note_data = data.read<QByteArray, quint16>();
		QByteArray encoding = data.read<QByteArray, quint16>();
		QTextCodec *codec = 0;
		if (!encoding.isEmpty()) {
			codec = QTextCodec::codecForName(encoding);
			if (!codec)
				debug() << "Server sent wrong encoding for status note";
		}
		if (!codec)
			codec = utf8Codec();
		status.setText(unescape(codec->toUnicode(note_data)));
//		debug() << "status note" << status.text();
	}
	// Updating capabilities
	Capabilities newCaps;
	DataUnit data(tlvs.value(0x000d));
	while (data.dataSize() >= 16) {
		Capability capability = data.read<Capability>();
		newCaps << capability;
	}
	if (tlvs.contains(0x0019)) {
		DataUnit data(tlvs.value(0x0019));
		while (data.dataSize() >= 2)
			newCaps.push_back(Capability(data.readData(2)));
	}
	contact->d_func()->setCapabilities(newCaps);
	if (tlvs.contains(0x000f))
		contact->d_func()->onlineSince = QDateTime::fromTime_t(QDateTime::currentDateTime().toTime_t() - tlvs.value<quint32>(0x000f));
	else if (tlvs.contains(0x0003))
		contact->d_func()->onlineSince = QDateTime::fromTime_t(tlvs.value<quint32>(0x0003));
	else
		contact->d_func()->onlineSince = QDateTime();

	if (tlvs.contains(0x0004))
		contact->d_func()->awaySince = QDateTime::currentDateTime().addSecs(-60 * tlvs.value<quint32>(0x0004));
	else if (statusId != OscarOnline)
		contact->d_func()->awaySince = QDateTime::currentDateTime();
	else
		contact->d_func()->awaySince = QDateTime();

	if (tlvs.contains(0x0005))
		contact->d_func()->regTime = QDateTime::fromTime_t(tlvs.value<quint32>(0x0005));
	else
		contact->d_func()->regTime = QDateTime();

	if (oldStatus == Status::Offline) {
		if (tlvs.contains(0x000c)) { // direct connection info
			DataUnit data(tlvs.value(0x000c));
			DirectConnectionInfo info =
			{
				QHostAddress(data.read<quint32>()),
				QHostAddress(),
				data.read<quint32>(),
				data.read<quint8>(),
				data.read<quint16>(),
				data.read<quint32>(),
				data.read<quint32>(),
				data.read<quint32>(),
				data.read<quint32>(),
				data.read<quint32>(),
				data.read<quint32>()
			};
			contact->d_func()->dc_info = info;
		}
	}
	setStatus(contact, status, tlvs);
}

void Roster::handleUserOffline(IcqAccount *account, const SNAC &snac)
{
	QString uin = snac.read<QString, quint8>();
	IcqContact *contact = account->getContact(uin);
	// We don't know this contact
	if (!contact)
		return;
	quint16 warning_level = snac.read<quint16>();
	Q_UNUSED(warning_level);
	TLVMap tlvs = snac.read<TLVMap, quint16>();
	//tlvs.value(0x0001); // User class
	contact->d_func()->clearCapabilities();
	OscarStatus status = contact->status();
	status.setType(Status::Offline);
	setStatus(contact, status, tlvs);
}

void Roster::reloadingStarted()
{
	Q_ASSERT(qobject_cast<Feedbag*>(sender()));
	Feedbag *feedbag = reinterpret_cast<Feedbag*>(sender());
	IcqAccountPrivate *d = feedbag->account()->d_func();
	d->connectingInfo.reset(new ConnectingInfo);
	// We will remove all contacts that still in the contact list
	// from this hash in Roster::handleAddModifyCLItem.
	d->connectingInfo->removedContacts = d->contacts;
	foreach (IcqContact *contact, feedbag->account()->contacts()) {
		d->connectingInfo->oldTags.insert(contact, contact->tags());
		contact->d_func()->items.clear();
		contact->d_func()->tags.clear();
	}
}

void Roster::loginFinished()
{
	Q_ASSERT(qobject_cast<IcqAccount*>(sender()));
	IcqAccount *account =  reinterpret_cast<IcqAccount*>(sender());
	IcqAccountPrivate *d = account->d_func();
	if (d->connectingInfo) {
		// Remove contacts that deleted from the server contact list.
		foreach (IcqContact *contact, d->connectingInfo->removedContacts)
			removeContact(contact);
		// Update contacts' tags.
		typedef QHash<IcqContact*, QStringList>::const_iterator OldTagsItr;
		OldTagsItr itr = d->connectingInfo->oldTags.constBegin();
		OldTagsItr end = d->connectingInfo->oldTags.constEnd();
		while (itr != end) {
			QStringList tags = itr.key()->tags();
			if (tags != itr.value()) // Tags are updated!
				emit itr.key()->tagsChanged(tags, itr.value());
			++itr;
		}
		foreach (IcqContact *contact, d->connectingInfo->createdContacts) {
			emit contact->inListChanged(true);
			emit contact->tagsChanged(contact->tags(), QStringList());
		}
		d->connectingInfo.reset();
	}
}

void Roster::accountAdded(qutim_sdk_0_3::Account *acc)
{
	Q_ASSERT(qobject_cast<IcqAccount*>(acc));
	IcqAccount *account = static_cast<IcqAccount*>(acc);
	connect(account->feedbag(), SIGNAL(reloadingStarted()), SLOT(reloadingStarted()));
	connect(account, SIGNAL(loginFinished()), SLOT(loginFinished()));
	account->connection()->registerInitializationSnac(BuddyFamily, UserCliReqBuddy);
}

void Roster::setStatus(IcqContact *contact, OscarStatus &status, const TLVMap &tlvs)
{
	foreach (RosterPlugin *plugin, contact->account()->d_func()->rosterPlugins)
		plugin->statusChanged(contact, status, tlvs);
	contact->setStatus(status);
//	debug() << contact->name() << "changed status to " << status.name();
}

RosterPlugin::~RosterPlugin()
{
}

} } // namespace qutim_sdk_0_3::oscar

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

#include "roster_p.h"
#include "icqcontact_p.h"
#include "icqaccount_p.h"
#include "icqprotocol.h"
#include "oscarconnection.h"
#include "buddypicture.h"
#include "buddycaps.h"
#include "messages.h"
#include "xtraz.h"
#include "feedbag.h"
#include "sessiondataitem.h"
#include <qutim/contactlist.h>
#include <qutim/messagesession.h>
#include <qutim/notificationslayer.h>
#include <qutim/actiongenerator.h>
#include <qutim/icon.h>

namespace qutim_sdk_0_3 {

namespace oscar {

using namespace Util;

PrivateListActionGenerator::PrivateListActionGenerator(quint16 type, const QIcon &icon,
				const LocalizedString &text1, const LocalizedString &text2) :
	ActionGenerator(icon, text1, new PrivateListActionHandler(), SLOT(onModifyPrivateList())),
	m_type(type), m_text2(text2)
{
}

PrivateListActionGenerator::~PrivateListActionGenerator()
{
	delete receiver();
}

QObject *PrivateListActionGenerator::generateHelper() const
{
	QAction *action = prepareAction(new QAction(NULL));
	IcqContact *contact = qobject_cast<IcqContact*>(action->data().value<MenuController*>());
	Q_ASSERT(contact);
	if (contact->account()->feedbag()->containsItem(m_type, contact->id()))
		action->setText(m_text2);
	action->setProperty("itemType", m_type);
	return action;
}

void PrivateListActionHandler::onModifyPrivateList()
{
	QAction *action = qobject_cast<QAction *>(sender());
	Q_ASSERT(action);
	quint16 type = action->property("itemType").toInt();
	IcqContact *contact = qobject_cast<IcqContact*>(action->data().value<MenuController*>());
	Q_ASSERT(contact);
	FeedbagItem item = contact->account()->feedbag()->item(type, contact->id(), 0, Feedbag::GenerateId);
	if (item.isInList())
		item.remove();
	else
		item.update();
}

typedef QSharedPointer<PrivateListActionGenerator> ActionPointer;
typedef QList<ActionPointer> ActionsList;
static void init_actions_list(ActionsList &list)
{
	list << ActionPointer(new PrivateListActionGenerator(SsiPermit, Icon("visible-icq"),
						  QT_TRANSLATE_NOOP("ContactList", "Add to visible list"),
						  QT_TRANSLATE_NOOP("ContactList", "Remove from visible list")))
		<< ActionPointer(new PrivateListActionGenerator(SsiDeny, Icon("invisible-icq"),
						 QT_TRANSLATE_NOOP("ContactList", "Add to invisible list"),
						 QT_TRANSLATE_NOOP("ContactList", "Remove from invisible list")))
		<< ActionPointer(new PrivateListActionGenerator(SsiIgnore, Icon("ignore-icq"),
						 QT_TRANSLATE_NOOP("ContactList", "Add to ignore list"),
						 QT_TRANSLATE_NOOP("ContactList", "Remove from ignore list")));
	foreach (const ActionPointer &action, list)
		MenuController::addAction<IcqContact>(action.data());
}
Q_GLOBAL_STATIC_WITH_INITIALIZER(ActionsList, actionsList, init_actions_list(*x));

Roster::Roster(IcqAccount *account):
	QObject(account)
{
	m_infos << SNACInfo(ServiceFamily, ServiceServerAsksServices)
			<< SNACInfo(ListsFamily, ListsError)
			<< SNACInfo(ListsFamily, ListsAuthRequest)
			<< SNACInfo(ListsFamily, ListsSrvAuthResponse)
			<< SNACInfo(ListsFamily, ListsList)
			<< SNACInfo(BuddyFamily, UserOnline)
			<< SNACInfo(BuddyFamily, UserOffline)
			<< SNACInfo(BuddyFamily, UserSrvReplyBuddy)
			<< SNACInfo(ExtensionsFamily, ExtensionsMetaError);
	m_types << SsiBuddy << SsiGroup << SsiPermit
			<< SsiDeny << SsiIgnore << SsiTags;
	connect(account, SIGNAL(statusChanged(qutim_sdk_0_3::Status)), SLOT(statusChanged(qutim_sdk_0_3::Status)));
	connect(account, SIGNAL(loginFinished()), SLOT(loginFinished()));
	m_account = account;
	m_conn = account->connection();
	Q_UNUSED(actionsList());
	account->feedbag()->registerHandler(this);
}

bool Roster::handleFeedbagItem(Feedbag *feedbag, const FeedbagItem &item, Feedbag::ModifyType type, FeedbagError error)
{
	if (error != FeedbagError::NoError)
		return false;
	if (type == Feedbag::Remove)
		handleRemoveCLItem(item);
	else
		handleAddModifyCLItem(item, type);
	return true;
}

void Roster::handleAddModifyCLItem(const FeedbagItem &item, Feedbag::ModifyType type)
{
	switch (item.type()) {
	case SsiBuddy: {
		if (item.name().isEmpty())
			break;
		bool creating = !m_account->contacts().contains(item.name());
		IcqContact *contact = m_account->getContact(item.name(), true);
		IcqContactPrivate *d = contact->d_func();
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
		if (newTag) {
			if (!creating && m_account->status() == Status::Connecting && d->items.isEmpty())
				loadTagsFromFeedbag(contact);
			d->items << item;
			emit contact->tagsChanged(contact->tags());
		}
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
		if (creating) {
			if (ContactList::instance()) {
				loadTagsFromFeedbag(contact);
				ContactList::instance()->addContact(contact);
			}
			debug().nospace() << "The contact " << contact->id() << " (" << contact->name() << ") has been added";
			return;
		} else {
			debug().nospace() << "The contact " << contact->id() << " (" << contact->name() << ") has been updated";
		}
		break;
	}
	case SsiGroup: {
		FeedbagItem old = m_account->feedbag()->groupItem(item.groupId());
		if (old.isInList() && old.name() != item.name()) {
			foreach (const FeedbagItem &i, m_account->feedbag()->group(item.groupId())) {
				QSet<QString> groups;
				IcqContact *contact = m_account->getContact(i.name());
				foreach (const FeedbagItem &i, contact->d_func()->items) {
					if (item.groupId() == i.groupId())
						groups << item.name();
					else
						groups << i.name();
				}
				foreach (const QString &tag,  contact->d_func()->tags)
					groups.insert(tag);
				emit contact->tagsChanged(groups);
			}
			debug(Verbose) << "The group" << old.name() << "has been renamed to" << item.name();
		} else {
			debug(Verbose) << "The group" << item.name() << "has been added";
		}
		break;
	}
	case SsiPermit: {
		debug() << item.name() << "has been added to visible list";
		break;
	}
	case SsiDeny: {
		debug() << item.name() << "has been added to invisible list";
		break;
	}
	case SsiIgnore: {
		debug() << item.name() << "has been added to ignore list";
		break;
	}
	case SsiTags: {
		IcqContact *contact = m_account->getContact(item.name());
		if (contact) {
			contact->d_func()->tags = readTags(item);
			emit contact->tagsChanged(contact->tags());
		}
		break;
	}
	}
}

void Roster::handleRemoveCLItem(const FeedbagItem &item)
{
	switch (item.type()) {
	case SsiBuddy: {
		IcqContact *contact = m_account->getContact(item.name());
		if (!contact) {
			warning() << "The contact" << item.name() << "does not exist";
			break;
		}
		removeContactFromGroup(contact, item.groupId());
		break;
	}
	case SsiGroup: {
		foreach (IcqContact *contact, m_account->contacts())
			removeContactFromGroup(contact, item.groupId());
		debug() << "The group" << item.name() << "has been removed";
		break;
	}
	case SsiPermit: {
		debug() << item.name() << "has been removed from visible list";
		break;
	}
	case SsiDeny: {
		debug() << item.name() << "has been removed from invisible list";
		break;
	}
	case SsiIgnore: {
		debug() << item.name() << "has been removed from ignore list";
		break;
	}
	case SsiTags: {
		IcqContact *contact = m_account->getContact(item.name());
		if (contact) {
			contact->d_func()->tags.clear();
			emit contact->tagsChanged(contact->tags());
		}
		break;
	}
	}
}

void Roster::removeContactFromGroup(IcqContact *contact, quint16 groupId)
{
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
			debug().nospace() << "The contact " << contact->id()
					<< " (" << contact->name() << ") has been removed";
			removeContact(contact);
		} else {
			debug().nospace() << "The contact " << contact->id() << " ("
					<< contact->name() << ") has been removed from "
					<< m_account->feedbag()->groupItem(groupId).name();
			emit contact->tagsChanged(contact->tags());
		}
	}
}

void Roster::loadTagsFromFeedbag(IcqContact *contact)
{
	FeedbagItem tagsItem = m_account->feedbag()->item(SsiTags, contact->id(), 0);
	if (tagsItem.isInList())
		contact->d_func()->tags = readTags(tagsItem);
}

void Roster::removeContact(IcqContact *contact)
{
/*
	if (ContactList::instance())
		ContactList::instance()->removeContact(contact);
	delete contact;
*/
	emit contact->tagsChanged(contact->tags());
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

void Roster::handleSNAC(AbstractConnection *c, const SNAC &sn)
{
	Q_ASSERT(c == m_conn);
	switch ((sn.family() << 16) | sn.subtype()) {
	case ServiceFamily << 16 | ServiceServerAsksServices: {
		quint16 buddyFlags = 0x0002;
		if (m_account->avatarsSupport()) {
			// Requesting avatar service
			SNAC snac(ServiceFamily, ServiceClientNewService);
			snac.append<quint16>(AvatarFamily);
			m_conn->send(snac);
			buddyFlags |= 0x0001;
		}

		// Requesting client-side contactlist rights
		SNAC snac(BuddyFamily, UserCliReqBuddy);
		// Query flags: 1 = Enable Avatars
		//              2 = Enable offline status message notification
		//              4 = Enable Avatars for offline contacts
		//              8 = Use reject for not authorized contacts
		snac.appendTLV<quint16>(0x05, buddyFlags); // mimic ICQ 6
		m_conn->send(snac);
		break;
	}
	case ListsFamily << 16 | ListsAuthRequest: {
		sn.skipData(8); // cookie
		QString uin = sn.read<QString, quint8>();
		QString reason = sn.read<QString, qint16>();
		debug() << QString("Authorization request from \"%1\" with reason \"%2").arg(uin).arg(reason);
		break;
	}
	case ListsFamily << 16 | ListsSrvAuthResponse: {
		sn.skipData(8); // cookie
		QString uin = sn.read<QString, qint8>();
		bool is_accepted = sn.read<qint8>();
		QString reason = sn.read<QString, qint16>();
		debug() << "Auth response" << uin << is_accepted << reason;
		break;
	}
	case ListsFamily << 16 | ListsList: {
		if (firstPacket) {
			firstPacket = false;
			foreach (IcqContact *contact, m_account->contacts()) {
				contact->d_func()->items.clear();
				contact->d_func()->tags.clear();
			}
		}
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

void Roster::handleUserOnline(const SNAC &snac)
{
	QString uin = snac.read<QString, quint8>();
	IcqContact *contact = m_account->getContact(uin);
	// We don't know this contact
	if (!contact)
		return;
	quint16 warning_level = snac.read<quint16>();
	Q_UNUSED(warning_level);
	TLVMap tlvs = snac.read<TLVMap, quint16>();
	// status.
	Status oldStatus = contact->status();
	quint16 statusFlags = 0;
	OscarStatus status(OscarOnline);
	if (tlvs.contains(0x06)) {
		DataUnit status_data(tlvs.value(0x06));
		statusFlags = status_data.read<quint16>();
		status = static_cast<OscarStatusEnum>(status_data.read<quint16>());
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
		status.setText(codec->toUnicode(note_data));
		debug() << "status note" << status.text();
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
	contact->setCapabilities(newCaps);
	
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

void Roster::handleUserOffline(const SNAC &snac)
{
	QString uin = snac.read<QString, quint8>();
	IcqContact *contact = m_account->getContact(uin);
	// We don't know this contact
	if (!contact)
		return;
	quint16 warning_level = snac.read<quint16>();
	Q_UNUSED(warning_level);
	TLVMap tlvs = snac.read<TLVMap, quint16>();
	//tlvs.value(0x0001); // User class
	contact->d_func()->clearCapabilities();
	OscarStatus status(OscarOffline);
	setStatus(contact, status, tlvs);
}

void Roster::statusChanged(qutim_sdk_0_3::Status status)
{
	if (status == Status::Connecting)
		firstPacket = true;
}

void Roster::loginFinished()
{
	foreach (IcqContact *contact, m_account->contacts()) {
		if (!m_account->feedbag()->containsItem(SsiBuddy, contact->id())) {
			if (ContactList::instance())
				ContactList::instance()->removeContact(contact);
			delete contact;
		} else {
			contact->d_func()->requestNick();
		}
	}
}

void Roster::setStatus(IcqContact *contact, OscarStatus &status, const TLVMap &tlvs)
{
	foreach (RosterPlugin *plugin, m_account->d_func()->rosterPlugins)
		plugin->statusChanged(contact, status, tlvs);
	contact->setStatus(status);
	debug() << contact->name() << "changed status to " << status.name();
}

RosterPlugin::~RosterPlugin()
{
}

} } // namespace qutim_sdk_0_3::oscar

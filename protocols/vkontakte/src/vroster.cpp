/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#include "vroster.h"
#include "vcontact.h"
#include "vaccount.h"
#include "vgroupchat.h"

#include <vk/roster.h>
#include <vk/longpoll.h>
#include <vk/message.h>

#include <qutim/chatsession.h>
#include <qutim/servicemanager.h>
#include <qutim/rosterstorage.h>
#include <QMetaProperty>

using namespace qutim_sdk_0_3;

class VRosterFactory : public ContactsFactory
{
public:
	VRosterFactory(VAccount *account, VRoster *roster) :
		account(account), roster(roster),
		addContactGuard(false)
	{
		rosterUpdater.setInterval(90000);
		roster->connect(&rosterUpdater, SIGNAL(timeout()), account->client()->roster(), SLOT(sync()));
	}

	virtual Contact *addContact(const QString &id, const QVariantMap &data)
	{
		VContact *c = roster->contact(id.toInt());
		vk::Contact::fillContact(c->buddy(), data);
		c->buddy()->setIsFriend(data.value("friend").toBool());
		return c;
	}
	virtual void serialize(Contact *obj, QVariantMap &data) {
		VContact *contact = qobject_cast<VContact*>(obj);
		if (!contact)
			return;
		vk::Buddy *buddy = contact->buddy();
		const QMetaObject *meta = buddy->metaObject();
		for (int i = 0; i != meta->propertyCount(); i++) {
			QMetaProperty property = meta->property(i);
			QString name = property.name();
			if (property.isStored(buddy) && name.leftRef(3) == "_q_") {
				name.remove(0, 3);
				data.insert(name, property.read(buddy));
			}
		}
		data.insert("friend", buddy->isFriend());
	}

	VAccount *account;
	VRoster *roster;
	ServicePointer<RosterStorage> storage;
	QHash<int, VContact*> contactHash;
	QHash<int, VGroupChat*> groupChatHash;
	bool addContactGuard;
	QTimer rosterUpdater;

	QString loadRoster();
};

VRoster::VRoster(VAccount *account) : QObject(account),
	p(new VRosterFactory(account, this))
{
	account->setContactsFactory(p.data());
	p->loadRoster();

	connect(p->account->client()->roster(), SIGNAL(buddyAdded(vk::Buddy*)), SLOT(onAddBuddy(vk::Buddy*)));
	connect(p->account->client()->roster(), SIGNAL(buddyUpdated(vk::Buddy*)), SLOT(onBuddyUpdated(vk::Buddy*)));
	connect(p->account->client()->roster(), SIGNAL(contactRemoved(int)), SLOT(onBuddyRemoved(int)));
	connect(p->account->client(), SIGNAL(onlineStateChanged(bool)), SLOT(onOnlineChanged(bool)));

	vk::LongPoll *poll = p->account->client()->longPoll();
	connect(poll, SIGNAL(messageAdded(vk::Message)), SLOT(onMessageAdded(vk::Message)));
	connect(poll, SIGNAL(contactTyping(int, int)), SLOT(onContactTyping(int, int)));
}

VRoster::~VRoster()
{
}

VContact *VRoster::contact(int id, bool create)
{
	VContact *c = p->contactHash.value(id);
	if (!c && create && id != p->account->uid()) {
		vk::Buddy *buddy = p->account->client()->roster()->buddy(id);
		c = createContact(buddy);
	}
	return c;
}

VContact *VRoster::contact(int id) const
{
	return p->contactHash.value(id);
}

VGroupChat *VRoster::groupChat(int id, bool create)
{
	VGroupChat *c = p->groupChatHash.value(id);
	if (!c && create) {
		c = new VGroupChat(p->account, id);
		connect(c, SIGNAL(destroyed(QObject*)), SLOT(onGroupChatDestroyed(QObject*)));
		p->groupChatHash.insert(id, c);
	}
	return c;
}

VGroupChat *VRoster::groupChat(int id) const
{
	return p->groupChatHash.value(id);
}

ContactsFactory *VRoster::contactsFactory() const
{
	return p.data();
}

VContact *VRoster::createContact(vk::Buddy *buddy)
{
	VContact *contact  = new VContact(buddy, p->account);
	connect(contact, SIGNAL(destroyed(QObject*)), SLOT(onContactDestroyed(QObject*)));
	p->contactHash.insert(buddy->id(), contact);
	emit p->account->contactCreated(contact);
	if (!p->addContactGuard)
		p->storage.data()->addContact(contact);
	return contact;
}

void VRoster::onAddBuddy(vk::Buddy *buddy)
{
	if (!p->contactHash.value(buddy->id())) {
		createContact(buddy);
		if (!buddy->isFriend())
			buddy->update(QStringList() << VK_COMMON_FIELDS);
	}
}

void VRoster::onBuddyUpdated(vk::Buddy *buddy)
{
	VContact *c = contact(buddy->id());
	p->storage.data()->updateContact(c);
}

void VRoster::onBuddyRemoved(int id)
{
	VContact *c = contact(id);
	p->storage.data()->removeContact(c);
}

void VRoster::onOnlineChanged(bool isOnline)
{
	if (isOnline) {
		vk::Reply *reply = p->account->client()->roster()->getMessages(0, 50, vk::Message::FilterUnread);
		connect(reply, SIGNAL(resultReady(QVariant)), SLOT(onMessagesRecieved(QVariant)));
		p->rosterUpdater.start();
	} else
		p->rosterUpdater.stop();
}

void VRoster::onMessageAdded(const vk::Message &msg)
{
	if (false) {
		int id = msg.isIncoming() ? msg.fromId() : msg.toId();
		VGroupChat *c = groupChat(id);
		if (c)
			c->handleMessage(msg);
	} else {
		int id = msg.isIncoming() ? msg.fromId() : msg.toId();
		VContact *c = contact(id);
		if (c)
			c->handleMessage(msg);
		else
			warning() << "Unable to find reciever with id in roster" << id;
	}
}

void VRoster::onContactDestroyed(QObject *obj)
{
	p->contactHash.remove(p->contactHash.key(static_cast<VContact*>(obj)));
}

void VRoster::onGroupChatDestroyed(QObject *obj)
{
	p->contactHash.remove(p->contactHash.key(static_cast<VContact*>(obj)));
}

void VRoster::onContactTyping(int userId, int chatId)
{
	if (!chatId) {
		VContact *c = contact(userId);
		c->setTyping(true);
	} else {
		VGroupChat *c = groupChat(chatId);
		//TODO
		//c->setChatState(ChatStateComposing);
	}
}

void VRoster::onMessagesRecieved(const QVariant &response)
{
	QVariantList list = response.toList();
	list.removeFirst();
	vk::MessageList msgList = vk::Message::fromVariantList(list, p->account->client());
	foreach (vk::Message msg, msgList)
		if (msg.isUnread() && msg.isIncoming())
			onMessageAdded(msg);
}


QString VRosterFactory::loadRoster()
{
	addContactGuard = true;
	QString version = storage->load(account);
	addContactGuard = false;
	return version;
}

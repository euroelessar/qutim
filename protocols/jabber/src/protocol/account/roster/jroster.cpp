/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#include "jroster.h"
#include "../jaccount.h"
#include "../../jprotocol.h"
#include "../muc/jmucmanager.h"
#include "../muc/jmucsession.h"
#include "jcontact.h"
#include "jcontactresource.h"
#include "../jpgpsupport.h"
#include <QFile>
#include <qutim/metacontact.h>
#include <qutim/metacontactmanager.h>
#include <QFile>
#include <QStringBuilder>
#include <qutim/authorizationdialog.h>
#include <qutim/notification.h>
#include <qutim/chatsession.h>
#include "../jaccount_p.h"
#include "jaccountresource.h"
#include <qutim/debug.h>
#include <qutim/rosterstorage.h>
#include <QApplication>
#include <jreen/pgpencrypted.h>
//Jreen
#include <jreen/chatstate.h>
#include <jreen/delayeddelivery.h>
#include <jreen/nickname.h>
#include <jreen/vcardupdate.h>

namespace Jabber
{

class JRosterPrivate : public ContactsFactory
{
public:
	JRosterPrivate(JRoster *q) : q_ptr(q) {}
	Contact *addContact(const QString &id, const QVariantMap &data);
	void serialize(Contact *contact, QVariantMap &data);
	
	JAccount *account;
	JRoster *q_ptr;
	RosterStorage *storage;
	QHash<QString, JContact*> contacts;
	QHash<QString, Jreen::MetaContactStorage::Item> metacontacts;
	Jreen::MetaContactStorage *metaStorage;
	bool showNotifications;
	bool ignoreChanges;
	bool atMetaLoad;
	bool atMetaSync;
};

static QEvent::Type metaContactSyncType()
{
	static QEvent::Type type = static_cast<QEvent::Type>(QEvent::registerEventType());
	return type;
}

Contact *JRosterPrivate::addContact(const QString &id, const QVariantMap &data)
{
	JContact *contact = new JContact(id, account);
	QObject::connect(contact, SIGNAL(destroyed(QObject*)), q_ptr, SLOT(onContactDestroyed(QObject*)));
	contact->setAvatar(data.value(QLatin1String("avatar")).toString());
	contact->setContactInList(true);
	contact->setContactName(data.value(QLatin1String("name")).toString());
	contact->setContactTags(data.value(QLatin1String("tags")).toStringList());
	int s10n = data.value(QLatin1String("s10n")).toInt();
	contact->setContactSubscription(static_cast<Jreen::RosterItem::SubscriptionType>(s10n));
	contact->setPGPKeyId(data.value(QLatin1String("pgpKeyId")).toString());
	contacts.insert(id, contact);
	emit account->contactCreated(contact);
	return contact;
}

void JRosterPrivate::serialize(Contact *generalContact, QVariantMap &data)
{
	JContact *contact = qobject_cast<JContact*>(generalContact);
	if (!contact)
		return;
	data.insert(QLatin1String("avatar"), contact->avatarHash());
	data.insert(QLatin1String("name"), contact->name());
	data.insert(QLatin1String("tags"), contact->tags());
	data.insert(QLatin1String("s10n"), contact->subscription());
	data.insert(QLatin1String("pgpKeyId"), contact->pgpKeyId());
}

JRoster::JRoster(JAccount *account) :
	AbstractRoster(account->client()),
	d_ptr(new JRosterPrivate(this))
{
	Q_D(JRoster);
	d->account = account;
	d->showNotifications = true;
	d->ignoreChanges = false;
	d->storage = RosterStorage::instance();
	d->account->setContactsFactory(d);
	d->metaStorage = new Jreen::MetaContactStorage(account->client());
	d->metaStorage->setPrivateXml(d->account->privateXml());
	d->atMetaLoad = false;
	d->atMetaSync = false;
	connect(d->metaStorage, SIGNAL(metaContactsReceived(Jreen::MetaContactStorage::ItemList)),
	        SLOT(onMetaContactsReceived(Jreen::MetaContactStorage::ItemList)));
	connect(d->account->client(),SIGNAL(presenceReceived(Jreen::Presence)),
			this,SLOT(handleNewPresence(Jreen::Presence)));
	connect(d->account->client(),SIGNAL(disconnected(Jreen::Client::DisconnectReason)),
			this,SLOT(onDisconnected()));
//	connect(d->account->client(),SIGNAL(messageReceived(Jreen::Message)),
//			this,SLOT(onNewMessage(Jreen::Message)));
}

JRoster::~JRoster()
{

}

void JRoster::loadFromStorage()
{
	Q_D(JRoster);
	QList<Jreen::RosterItem::Ptr> items;
	d->ignoreChanges = true;
	QString version = d->storage->load(d->account);
	QHashIterator<QString, JContact*> contacts = d->contacts;
	while (contacts.hasNext()) {
		contacts.next();
		JContact *contact = contacts.value();
		items << Jreen::RosterItem::Ptr(new Jreen::RosterItem(
				contact->id(), contact->name(), contact->tags(), contact->subscription()));
	}
	fillRoster(version, items);
	d->ignoreChanges = false;
}

bool JRoster::ignoreChanges() const
{
	return d_func()->ignoreChanges;
}

void JRoster::onItemAdded(QSharedPointer<Jreen::RosterItem> item)
{
	Q_D(JRoster);
	if (d->ignoreChanges)
		return;
	JContact *contact = static_cast<JContact*>(JRoster::contact(item->jid(), true));
	Q_ASSERT(contact);
	fillContact(contact, item);
	d->storage->addContact(contact, version());
	if(d->showNotifications) {
		NotificationRequest request(Notification::System);
		request.setObject(contact);
		request.setText(tr("Contact %1 has been added to roster").arg(contact->title()));
		request.send();
	}
}
void JRoster::onItemUpdated(QSharedPointer<Jreen::RosterItem> item)
{
	Q_D(JRoster);
	if (d->ignoreChanges)
		return;
	if (JContact *contact = d->contacts.value(item->jid())) {
		fillContact(contact, item);
		d->storage->updateContact(contact, version());
	}
}

void JRoster::onItemRemoved(const QString &jid)
{
	Q_D(JRoster);
	if (d->ignoreChanges)
		return;
	JContact *contact = d->contacts.take(jid);
	if(!contact)
		return;
	d->storage->removeContact(contact, version());
	contact->setContactInList(false);
	contact->setContactSubscription(Jreen::RosterItem::None);
	if(d->showNotifications) {
		NotificationRequest request(Notification::System);
		request.setObject(contact);
		request.setText(tr("Contact %1 has been removed from roster").arg(contact->title()));
		request.send();
	}
}

void JRoster::onLoaded(const QList<QSharedPointer<Jreen::RosterItem> > &items)
{
	Q_D(JRoster);
	d->showNotifications = false;
	AbstractRoster::onLoaded(items);
	d->showNotifications = true;
	d->metaStorage->requestMetaContacts();
}

JContact *JRoster::createContact(const Jreen::JID &id)
{
	Q_D(JRoster);
	JContact *contact = new JContact(id.bare(),d->account);
	d->contacts.insert(id.bare(),contact);
	contact->setContactInList(false);
	emit d->account->contactCreated(contact);
	connect(contact,SIGNAL(destroyed(QObject*)),SLOT(onContactDestroyed(QObject*)));
	return contact;
}

ChatUnit *JRoster::contact(const Jreen::JID &jid, bool create)
{
	Q_D(JRoster);
	QString bare = jid.bare();
	if (bare == d->account->client()->jid().bare())
		bare = jid.full();
	QString resourceId = jid.resource();
	JContact *contact = d->contacts.value(bare);
	if (!resourceId.isEmpty()) {
		if (!contact) {
			if (create)
				return createContact(jid);
			else {
				return 0;
			}
		}
		if (JContactResource *resource = contact->resource(resourceId))
			return resource;
		if (create) {
			return contact;
		}
	} else if (contact) {
		return contact;
	} else if (create) {
		return createContact(jid);
	}
	return 0;
}

ChatUnit *JRoster::selfContact(const QString &id)
{
	return d_func()->contacts.value(id);
}

QList<JContactResource *> JRoster::resources() const
{
	Q_D(const JRoster);
	QList<JContactResource *> result;
	foreach (JContact *contact, d->contacts)
		result += contact->resources();
	return result;
}

void JRoster::fillContact(JContact *contact, QSharedPointer<Jreen::RosterItem> item)
{
	QString name = item->name();
	contact->setContactName(name);
	QStringList tags = item->groups();
	contact->setContactTags(tags);
	if (!contact->isInList())
		contact->setContactInList(true);
	contact->setContactSubscription(item->subscription());
}

void JRoster::handleNewPresence(Jreen::Presence presence)
{
	Q_D(JRoster);

	switch(presence.subtype())
	{
	case Jreen::Presence::Subscribe:
	case Jreen::Presence::Unsubscribe:
	case Jreen::Presence::Unsubscribed:
	case Jreen::Presence::Subscribed:
		handleSubscription(presence);
		return;
	case Jreen::Presence::Error:
	case Jreen::Presence::Probe:
		return;
	default:
		break;
	}

	const Jreen::JID self = d->account->client()->jid();
	const Jreen::JID from = presence.from();
	if (self == from) 
		d->account->d_func()->setPresence(presence);
	else if (self.bare() == from.bare())
		handleSelfPresence(presence);
	else if (JContact *c = d->contacts.value(from.bare()))
		c->setStatus(presence);
}

void JRoster::handleSelfPresence(Jreen::Presence presence)
{
	Q_D(JRoster);
	JContact * &contact = d->contacts[presence.from().full()];
	bool contactCreated = false;
	if (presence.subtype() == Jreen::Presence::Unavailable) {
		bool hasSession = false;
		if (contact) {
			if (ChatSession *session = ChatLayer::get(contact, false)) {
				hasSession = true;
				connect(session, SIGNAL(destroyed()), contact, SLOT(deleteLater()));
			}
		}
		if (!hasSession) {
			d->contacts.remove(presence.from().full());
			delete contact;
			contact = 0;
		}
	} else {
		if (!contact) {
			contact = new JAccountResource(d->account, presence.from().full(), presence.from().resource());
			contactCreated = true;
		}
		if (ChatSession *session = ChatLayer::get(contact, false))
			disconnect(session, SIGNAL(destroyed()), contact, SLOT(deleteLater()));
	}
	if (contact)
		contact->setStatus(presence);
	if (contactCreated)
		emit d->account->contactCreated(contact);
}

void JRoster::syncMetaContacts()
{
	Q_D(JRoster);
	if (!d->atMetaSync) {
		qApp->postEvent(this, new QEvent(metaContactSyncType()), Qt::LowEventPriority);
		d->atMetaSync = true;
	}
}

void JRoster::onDisconnected()
{
	Q_D(JRoster);
	foreach (JContact *c, d->contacts) {
		Jreen::Presence unavailable(Jreen::Presence::Unavailable, c->id());
		if (qobject_cast<JAccountResource*>(c))
			handleSelfPresence(unavailable);
		else
			c->setStatus(unavailable);
	}
}

void JRoster::onNewMessage(Jreen::Message message)
{
	Q_D(JRoster);
	
	if(message.body().isEmpty())
		return;
	
	//temporary
	ChatUnit *chatUnit = 0;
	ChatUnit *unitForSession = 0;
	ChatUnit *muc = d->account->conferenceManager()->muc(message.from().bareJID());
	if (muc) {
		JMUCSession *session = static_cast<JMUCSession*>(muc);
		chatUnit = session->participant(message.from().resource());
		unitForSession = chatUnit;
	} else {
		JContact *contact = d->contacts.value(message.from().full());
		if (!contact)
			contact = d->contacts.value(message.from().bare());
		chatUnit = contact ? JRoster::contact(message.from(), false) : 0;
		if (!contact) {
			contact = static_cast<JContact*>(JRoster::contact(message.from(),true));
			contact->setInList(false);
			if(Jreen::Nickname::Ptr nick = message.payload<Jreen::Nickname>())
				contact->setName(nick->nick());
		}
		if (!chatUnit)
			chatUnit = contact;
		unitForSession = contact;
	}
	
	if (JPGPDecryptReply *reply = JPGPSupport::instance()->decrypt(chatUnit, unitForSession, message)) {
		connect(reply, SIGNAL(finished(ChatUnit*,ChatUnit*,Jreen::Message)),
		        SLOT(onMessageDecrypted(ChatUnit*,ChatUnit*,Jreen::Message)));
	} else {
		onMessageDecrypted(unitForSession, chatUnit, message);
	}
}

void JRoster::onMessageDecrypted(ChatUnit *chatUnit, ChatUnit *unitForSession, const Jreen::Message &message)
{
	if (!chatUnit && !unitForSession)
		return;
	if (!chatUnit)
		chatUnit = unitForSession;
	if (!unitForSession)
		unitForSession = chatUnit;
	qutim_sdk_0_3::Message coreMessage;
	if(Jreen::DelayedDelivery::Ptr d = message.when())
		coreMessage.setTime(d->dateTime());
	else
		coreMessage.setTime(QDateTime::currentDateTime());
	coreMessage.setText(message.body());
	coreMessage.setProperty("subject",message.subject());
	coreMessage.setChatUnit(chatUnit);
	coreMessage.setIncoming(true);
	if (message.payload<Jreen::PGPEncrypted>())
		coreMessage.setProperty("pgpEncrypted", true);
	ChatLayer::get(unitForSession, true)->appendMessage(coreMessage);
}

void JRoster::handleSubscription(Jreen::Presence subscription)
{
	Q_D(JRoster);
	QString bare = subscription.from().bare();
	QString name;
	JContact *contact = d->contacts.value(bare);
	if (contact) {
		name = contact->name();
	} else {
		Jreen::Nickname *nickname = subscription.payload<Jreen::Nickname>().data();
		if(nickname)
			name = nickname->nick();
	}

	switch(subscription.subtype())
	{
	case Jreen::Presence::Subscribe: {
		contact = static_cast<JContact*>(this->contact(bare,true));
		contact->setContactName(name);
		contact->setContactInList(false);
		Authorization::Reply request(Authorization::Reply::New,
									 contact,
									 subscription.status());
		qApp->sendEvent(Authorization::service(),&request);
		break;
	}
	case Jreen::Presence::Unsubscribe: { //how to handle it?
		QString text = tr("Received a request for removal from the subscribers");
		Authorization::Reply request(Authorization::Reply::Reject,
									 contact,
									 text);
		qApp->sendEvent(Authorization::service(),&request);
		break;
	}
	case Jreen::Presence::Unsubscribed: {
		QString text = tr("You have been removed from the list of subscribers");
		Authorization::Reply request(Authorization::Reply::Rejected,
									 contact,
									 text);
		qApp->sendEvent(Authorization::service(),&request);
		break;
	}
	case Jreen::Presence::Subscribed: {
		QString text = tr("You have been added to the list of subscribers");
		Authorization::Reply request(Authorization::Reply::Accepted,
									 contact,
									 text);
		qApp->sendEvent(Authorization::service(),&request);
		break;
	}
	default:
		break;
	}
}

void JRoster::addContact(const JContact *contact)
{
	Q_D(JRoster);
	add(contact->id(),contact->name(),contact->tags());
	Jreen::Presence presence (Jreen::Presence::Subscribed,
							  contact->id()
							  );
	d->account->client()->send(presence);
}

void JRoster::removeContact(const JContact *contact)
{
	Q_D(JRoster);
	remove(contact->id());
	removeSubscription(contact);
	Jreen::Presence presence (Jreen::Presence::Unsubscribed,
							  contact->id()
							  );
	d->account->client()->send(presence);
}

void JRoster::onContactDestroyed(QObject *obj)
{
	Q_D(JRoster);
	JContact *c = static_cast<JContact*>(obj);
	d->contacts.remove(d->contacts.key(c));
}

void JRoster::onMetaContactsReceived(const Jreen::MetaContactStorage::ItemList &items)
{
	Q_D(JRoster);
	d->atMetaLoad = true;
	QSet<QString> removedContacts = QSet<QString>::fromList(d->metacontacts.keys());
	foreach (const Jreen::MetaContactStorage::Item &item, items) {
		JContact *contact = d->contacts.value(item.jid().bare());
        if (!contact)
            continue;
        MetaContact *metaContact = qobject_cast<MetaContact*>(contact->metaContact());
		removedContacts.remove(item.jid().bare());
        if (metaContact && metaContact->id() == item.tag())
            continue;
        ChatUnit *unit = MetaContactManager::instance()->getUnit(item.tag(), true);
        metaContact = qobject_cast<MetaContact*>(unit);
        Q_ASSERT(metaContact);
        metaContact->addContact(contact);
		d->metacontacts.insert(contact->id(), item);
    }
	foreach (const QString &jid, removedContacts) {
		JContact *contact = d->contacts.value(jid);
		MetaContact *metaContact = qobject_cast<MetaContact*>(contact->metaContact());
		Q_ASSERT(metaContact);
		metaContact->removeContact(contact);
		d->metacontacts.remove(jid);
	}
	d->atMetaLoad = false;
}

void JRoster::removeSubscription(const JContact *contact)
{
	Q_D(JRoster);
	Jreen::Presence presence(Jreen::Presence::Unsubscribe,
							 contact->id()
							 );
	d->account->client()->send(presence);
	synchronize();
}

void JRoster::requestSubscription(const Jreen::JID &id,const QString &reason)
{
	Q_D(JRoster);
	Jreen::Presence presence(Jreen::Presence::Subscribe,
							 id,
							 reason
							 );
	d->account->client()->send(presence);
}

void JRoster::loadSettings()
{

}

void JRoster::saveSettings()
{

}

void JRoster::setGroups(const JContact *contact, const QStringList &groups)
{
	if (Jreen::RosterItem::Ptr i = item(contact->id())) {
		i->setGroups(groups);
		synchronize();
	}
}

void JRoster::handleChange(JContact *contact, const QString &metaTag)
{
	Q_D(JRoster);
	if (d->atMetaLoad)
		return;

	if (metaTag.isEmpty()) {
		d->metacontacts.remove(contact->id());
	} else {
		Jreen::MetaContactStorage::Item item;
		item.setJID(contact->id());
		item.setTag(metaTag);
		d->metacontacts.insert(contact->id(), item);
	}
	if (d->account->client()->isConnected())
		syncMetaContacts();
}

bool JRoster::event(QEvent *ev)
{
	if (ev->type() == metaContactSyncType()) {
		Q_D(JRoster);
		d->metaStorage->storeMetaContacts(d->metacontacts.values());
		d->atMetaSync = false;
		return true;
	}
	return Jreen::AbstractRoster::event(ev);
}

void JRoster::setName(const JContact *contact, const QString &name)
{
	if (Jreen::RosterItem::Ptr i = item(contact->id())) {
		i->setName(name);
		synchronize();
	}
}

} //namespace jabber


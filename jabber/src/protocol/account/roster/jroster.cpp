#include "jroster.h"
#include "../jaccount.h"
#include "../../jprotocol.h"
#include "jcontact.h"
#include "jcontactresource.h"
#include "../vcard/jvcardmanager.h"
#include <QFile>
#include <qutim/metacontact.h>
#include <qutim/metacontactmanager.h>
#include <QFile>
#include <QStringBuilder>
#include <qutim/authorizationdialog.h>
#include <qutim/notificationslayer.h>
#include <qutim/messagesession.h>
#include "../jaccount_p.h"
#include <qutim/debug.h>
#include <qutim/notificationslayer.h>
#include <qutim/rosterstorage.h>
#include <QApplication>
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
	bool showNotifications;
	bool ignoreChanges;
};

Contact *JRosterPrivate::addContact(const QString &id, const QVariantMap &data)
{
	JContact *contact = new JContact(id, account);
	QObject::connect(contact, SIGNAL(destroyed(QObject*)), q_ptr, SLOT(onContactDestroyed(QObject*)));
	contact->setContactInList(true);
	contact->setAvatar(data.value(QLatin1String("avatar")).toString());
	contact->setContactName(data.value(QLatin1String("name")).toString());
	contact->setContactTags(data.value(QLatin1String("tags")).toStringList());
	int s10n = data.value(QLatin1String("s10n")).toInt();
	contact->setContactSubscription(static_cast<Jreen::AbstractRosterItem::SubscriptionType>(s10n));
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
	connect(d->account->client(),SIGNAL(newPresence(Jreen::Presence)),
			this,SLOT(handleNewPresence(Jreen::Presence)));
	connect(d->account->client(),SIGNAL(disconnected(Jreen::Client::DisconnectReason)),
			this,SLOT(onDisconnected()));
//	connect(d->account->client(),SIGNAL(newMessage(Jreen::Message)),
//			this,SLOT(onNewMessage(Jreen::Message)));
}

JRoster::~JRoster()
{

}

void JRoster::loadFromStorage()
{
	Q_D(JRoster);
	QList<Jreen::AbstractRosterItem::Ptr> items;
	QString version = d->storage->load(d->account);
	QHashIterator<QString, JContact*> contacts = d->contacts;
	while (contacts.hasNext()) {
		contacts.next();
		JContact *contact = contacts.value();
		items << Jreen::AbstractRosterItem::Ptr(new Jreen::AbstractRosterItem(
				contact->id(), contact->name(), contact->tags(), contact->subscription()));
	}
	d->ignoreChanges = true;
	fillRoster(version, items);
	d->ignoreChanges = false;
}

void JRoster::onItemAdded(QSharedPointer<Jreen::AbstractRosterItem> item)
{
	Q_D(JRoster);
	if (d->ignoreChanges)
		return;
	JContact *contact = static_cast<JContact*>(JRoster::contact(item->jid(), true));
	Q_ASSERT(contact);
	fillContact(contact, item);
	d->storage->addContact(contact, version());
	if(d->showNotifications) {
		Notifications::send(Notification::System,
							contact,
							tr("Contact %1 has been added to roster").arg(contact->title()));
	}
}
void JRoster::onItemUpdated(QSharedPointer<Jreen::AbstractRosterItem> item)
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
	contact->setContactSubscription(Jreen::AbstractRosterItem::None);
	if(d->showNotifications) {
		Notifications::send(Notification::System,
							contact,
							tr("Contact %1 has been removed from roster").arg(contact->title()));
	}
}

void JRoster::onLoaded(const QList<QSharedPointer<Jreen::AbstractRosterItem> > &items)
{
	Q_D(JRoster);
	d->showNotifications = false;
	AbstractRoster::onLoaded(items);
	d->showNotifications = true;
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
	} else if (create)
		return createContact(jid);
	return 0;
}

void JRoster::fillContact(JContact *contact, QSharedPointer<Jreen::AbstractRosterItem> item)
{
	QString name = item->name();
	contact->setContactName(name);
	QStringList tags = item->groups();
	contact->setContactTags(tags);
	if (!contact->isInList())
		contact->setContactInList(true);
	contact->setContactSubscription(item->subscriptionType());
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
		break;
	case Jreen::Presence::Error:
	case Jreen::Presence::Probe:
		return;
	default:
		break;
	}

	const Jreen::Error *error = presence.error();
	Jreen::JID from = presence.from();
	if(d->account->client()->jid() == from) {
		d->account->d_func()->setPresence(presence);
		return;
	}
	JContact *c = d->contacts.value(from.bare());
	if (c) {
		c->setStatus(presence);		
		Jreen::VCardUpdate::Ptr vcard = presence.findExtension<Jreen::VCardUpdate>();
		if(vcard && vcard->hasPhotoInfo() && !error) {
			QString hash = vcard->photoHash();
			debug() << "vCard update" << (c->avatarHash() != hash) << c->id() << c->avatarHash() << hash;
			if (c->avatarHash() != hash) {
				if(hash.isEmpty() || QFile(d->account->getAvatarPath()%QLatin1Char('/')%hash).exists()) {
					c->setAvatar(hash);
					if (c->isInList())
						d->storage->updateContact(c, version());
				} else {
					d->account->d_ptr->vCardManager->fetchVCard(from);
				}
			}
		}
	}
}

void JRoster::onDisconnected()
{
	Q_D(JRoster);
	foreach (JContact *c, d->contacts) {
		Jreen::Presence unavailable(Jreen::Presence::Unavailable, c->id());
		c->setStatus(unavailable);
	}
}

void JRoster::onNewMessage(Jreen::Message message)
{
	Q_D(JRoster);
	//temporary
	JContact *contact = d->contacts.value(message.from().bare());
	ChatUnit *chatUnit = contact ? JRoster::contact(message.from().full(), false) : 0;
	if(!contact) {
		contact = static_cast<JContact*>(JRoster::contact(message.from(),true));
		contact->setInList(false);
		if(Jreen::Nickname::Ptr nick = message.findExtension<Jreen::Nickname>())
			contact->setName(nick->nick());
		chatUnit = contact;
	}

	if(message.body().isEmpty())
		return;
	qutim_sdk_0_3::Message coreMessage;
	if(const Jreen::DelayedDelivery *d = message.when())
		coreMessage.setTime(d->dateTime());
	else
		coreMessage.setTime(QDateTime::currentDateTime());
	coreMessage.setText(message.body());
	coreMessage.setProperty("subject",message.subject());
	coreMessage.setChatUnit(chatUnit);
	coreMessage.setIncoming(true);
	ChatLayer::get(contact,true)->appendMessage(coreMessage);
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
		Jreen::Nickname *nickname = subscription.findExtension<Jreen::Nickname>().data();
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
	if (Jreen::AbstractRosterItem::Ptr i = item(contact->id())) {
		i->setGroups(groups);
		synchronize();
	}
}

void JRoster::setName(const JContact *contact, const QString &name)
{
	if (Jreen::AbstractRosterItem::Ptr i = item(contact->id())) {
		i->setName(name);
		synchronize();
	}
}

} //namespace jabber

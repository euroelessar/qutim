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
#include <QApplication>
//jreen
#include <jreen/chatstate.h>
#include <jreen/delayeddelivery.h>
#include <jreen/nickname.h>
#include <jreen/vcardupdate.h>

namespace Jabber
{

class JRosterPrivate
{
public:
	JRosterPrivate(JRoster *q) : q_ptr(q) {}
	JAccount *account;
	JRoster *q_ptr;
	QHash<QString, JContact*> contacts;
	bool isLoaded;
};

JRoster::JRoster(JAccount *account) :
	AbstractRoster(account->client()),
	d_ptr(new JRosterPrivate(this))
{
	Q_D(JRoster);
	d->account = account;
	d->isLoaded = false;
	connect(d->account->client(),SIGNAL(newPresence(jreen::Presence)),
			this,SLOT(handleNewPresence(jreen::Presence)));
	connect(d->account->client(),SIGNAL(disconnected()),
			this,SLOT(onDisconnected()));
//	connect(d->account->client(),SIGNAL(newMessage(jreen::Message)),
//			this,SLOT(onNewMessage(jreen::Message)));
}

JRoster::~JRoster()
{

}

void JRoster::onItemAdded(QSharedPointer<jreen::AbstractRosterItem> item)
{
	//Q_D(JRoster);
	JContact *contact = static_cast<JContact*>(JRoster::contact(item->jid(), true));
	Q_ASSERT(contact);
	fillContact(contact, item);
	if(d_func()->isLoaded) {
		Notifications::send(Notifications::System,
							contact,
							tr("Contact %1 has been added to roster").arg(contact->title()));
	}
}
void JRoster::onItemUpdated(QSharedPointer<jreen::AbstractRosterItem> item)
{
	Q_D(JRoster);
	if (JContact *contact = d->contacts.value(item->jid()))
		fillContact(contact, item);
}

void JRoster::onItemRemoved(const QString &jid)
{
	Q_D(JRoster);
	JContact *contact = d->contacts.take(jid);
	if(!contact)
		return;
	contact->setContactInList(false);
	contact->setContactSubscription(jreen::AbstractRosterItem::None);
	if(d->isLoaded) {
		Notifications::send(Notifications::System,
							contact,
							tr("Contact %1 has been removed from roster").arg(contact->title()));
	}
}

void JRoster::onLoaded(const QList<QSharedPointer<jreen::AbstractRosterItem> > &items)
{
	Q_D(JRoster);
	d->isLoaded = false;
	AbstractRoster::onLoaded(items);
	d->isLoaded = true;
}

JContact *JRoster::createContact(const jreen::JID &id)
{
	Q_D(JRoster);
	JContact *contact = new JContact(id.bare(),d->account);
	d->contacts.insert(id.bare(),contact);
	contact->setContactInList(false);
	emit d->account->contactCreated(contact);
	connect(contact,SIGNAL(destroyed(QObject*)),SLOT(onContactDestroyed(QObject*)));
	return contact;
}

ChatUnit *JRoster::contact(const jreen::JID &jid, bool create)
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

void JRoster::fillContact(JContact *contact, QSharedPointer<jreen::AbstractRosterItem> item)
{
	QString name = item->name();
	contact->setContactName(name);
	QStringList tags = item->groups();
	contact->setContactTags(tags);
	if (!contact->isInList())
		contact->setContactInList(true);
	contact->setContactSubscription(item->subscriptionType());
}

void JRoster::handleNewPresence(jreen::Presence presence)
{
	Q_D(JRoster);

	switch(presence.subtype())
	{
	case jreen::Presence::Subscribe:
	case jreen::Presence::Unsubscribe:
	case jreen::Presence::Unsubscribed:
	case jreen::Presence::Subscribed:
		handleSubscription(presence);
		break;
	case jreen::Presence::Error:
	case jreen::Presence::Probe:
		return;
	default:
		break;
	}

	jreen::JID from = presence.from();
	if(d->account->client()->jid() == from) {
		d->account->d_func()->setPresence(presence);
		return;
	}
	JContact *c = d->contacts.value(from.bare());
	if (c) {
		c->setStatus(presence);		
		jreen::VCardUpdate::Ptr vcard = presence.findExtension<jreen::VCardUpdate>();
		if(vcard) {
			QString hash = vcard->photoHash();
			debug() << "vCard update" << (c->avatarHash() != hash);
			if (c->avatarHash() != hash) {
				if(hash.isEmpty() || QFile(d->account->getAvatarPath()%QLatin1Char('/')%hash).exists())
					c->setAvatar(hash);
				else
					d->account->d_ptr->vCardManager->fetchVCard(from);
			}
		}
	}
}

void JRoster::onDisconnected()
{
	Q_D(JRoster);
	foreach (JContact *c, d->contacts) {
		jreen::Presence unavailable(jreen::Presence::Unavailable, c->id());
		c->setStatus(unavailable);
	}
}

void JRoster::onNewMessage(jreen::Message message)
{
	Q_D(JRoster);
	//temporary
	JContact *c = d->contacts.value(message.from().bare());
	if(!c) {
		c = static_cast<JContact*>(contact(message.from(),true));
		c->setInList(false);
		if(jreen::Nickname *nick = message.findExtension<jreen::Nickname>().data())
			c->setName(nick->nick());
	}

	if(message.body().isEmpty())
		return;
	qutim_sdk_0_3::Message coreMessage;
	if(const jreen::DelayedDelivery *d = message.when())
		coreMessage.setTime(d->dateTime());
	else
		coreMessage.setTime(QDateTime::currentDateTime());
	coreMessage.setText(message.body());
	coreMessage.setProperty("subject",message.subject());
	coreMessage.setChatUnit(c);
	coreMessage.setIncoming(true);
	ChatLayer::get(c,true)->appendMessage(coreMessage);
}

void JRoster::handleSubscription(jreen::Presence subscription)
{
	Q_D(JRoster);
	QString bare = subscription.from().bare();
	QString name;
	JContact *contact = d->contacts.value(bare);
	if (contact) {
		name = contact->name();
	} else {
		jreen::Nickname *nickname = subscription.findExtension<jreen::Nickname>().data();
		if(nickname)
			name = nickname->nick();
	}

	switch(subscription.subtype())
	{
	case jreen::Presence::Subscribe: {
		contact = static_cast<JContact*>(this->contact(bare,true));
		contact->setContactName(name);
		contact->setContactInList(false);
		Authorization::Reply request(Authorization::Reply::New,
									 contact,
									 subscription.status());
		qApp->sendEvent(Authorization::service(),&request);
		break;
	}
	case jreen::Presence::Unsubscribe: { //how to handle it?
		QString text = tr("Received a request for removal from the subscribers");
		Authorization::Reply request(Authorization::Reply::Reject,
									 contact,
									 text);
		qApp->sendEvent(Authorization::service(),&request);
		break;
	}
	case jreen::Presence::Unsubscribed: {
		QString text = tr("You have been removed from the list of subscribers");
		Authorization::Reply request(Authorization::Reply::Rejected,
									 contact,
									 text);
		qApp->sendEvent(Authorization::service(),&request);
		break;
	}
	case jreen::Presence::Subscribed: {
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
	jreen::Presence presence (jreen::Presence::Subscribed,
							  contact->id()
							  );
	d->account->client()->send(presence);
}

void JRoster::removeContact(const JContact *contact)
{
	Q_D(JRoster);
	remove(contact->id());
	removeSubscription(contact);
	jreen::Presence presence (jreen::Presence::Unsubscribed,
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
	jreen::Presence presence(jreen::Presence::Unsubscribe,
							 contact->id()
							 );
	d->account->client()->send(presence);
	synchronize();
}

void JRoster::requestSubscription(const jreen::JID &id,const QString &reason)
{
	Q_D(JRoster);
	jreen::Presence presence(jreen::Presence::Subscribe,
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
	m_items.value(contact->id())->setGroups(groups);
	synchronize();
}

void JRoster::setName(const JContact *contact, const QString &name)
{
	m_items.value(contact->id())->setName(name);
	synchronize();
}

} //namespace jabber

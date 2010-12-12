#include "jcontact.h"
#include "jcontactresource.h"
#include "../vcard/jinforequest.h"
#include "../jaccount.h"
#include "../../jprotocol.h"
#include <gloox/message.h>
#include <gloox/chatstate.h>
#include <gloox/rostermanager.h>
#include <gloox/rosteritem.h>
#include <QStringBuilder>
#include "qutim/tooltip.h"
#include "qutim/extensionicon.h"
#include <qutim/inforequest.h>
#include <qutim/debug.h>
#include <qutim/message.h>
#include "jmessagesession.h"
#include "jmessagehandler.h"
#include <qutim/metacontact.h>
#include <qutim/authorizationdialog.h>
//jreen
#include <jreen/presence.h>
#include <jreen/client.h>
#include <jreen/chatstate.h>
#include <jreen/delayeddelivery.h>
#include <jreen/receipt.h>
#include "jroster.h"
#include "../vcard/jvcardmanager.h"

using namespace gloox;

namespace Jabber
{

//testing
class JMessageReceiptFilter : public jreen::MessageFilter
{
public:
	JMessageReceiptFilter(JContact *contact,jreen::MessageSession *session)
		: jreen::MessageFilter(session),
		  m_contact(contact)
	{

	}
	virtual ~JMessageReceiptFilter() {}
	virtual void filter(jreen::Message &message)
	{
		jreen::Receipt *receipt = message.findExtension<jreen::Receipt>().data();
		if(receipt) {
			if(receipt->type() == jreen::Receipt::Received) {
				QString id = receipt->id();
				if(id.isEmpty())
					id = message.id(); //for slowpoke client such as Miranda
				qApp->postEvent(ChatLayer::get(m_contact),
								new qutim_sdk_0_3::MessageReceiptEvent(id.toUInt(), true));
			} else {
				//only for testing
				//TODO send this request only when message marked as read
				jreen::Message request(jreen::Message::Chat,
									   message.from());
				request.addExtension(new jreen::Receipt(jreen::Receipt::Received,message.id()));
				static_cast<JAccount*>(m_contact->account())->client()->send(request);
			}
		}
	}
	virtual void decorate(jreen::Message &message)
	{
		jreen::Receipt *receipt = new jreen::Receipt(jreen::Receipt::Request);
		message.addExtension(receipt);
	}
	virtual void reset()
	{

	}
	virtual int filterType() const
	{
		return 1;
	}
private:
	JContact *m_contact;
};

class JContactPrivate
{
public:
	JAccount *account;
	QHash<QString, JContactResource *> resources;
	QStringList currentResources;
	QStringList tags;
	QString name;
	QString jid;
	bool inList;
	QString avatar;
	QStringRef hash;
	QHash<QString, QVariantHash> extInfo;
	jreen::AbstractRosterItem::SubscriptionType subscription;
};

JContact::JContact(const QString &jid, JAccount *account) : Contact(account), d_ptr(new JContactPrivate)
{
	Q_D(JContact);
	d->account = account;
	d->jid = jid;
}

JContact::~JContact()
{
	qDeleteAll(d_func()->resources);
}

QString JContact::id() const
{
	return d_func()->jid;
}

bool JContact::sendMessage(const qutim_sdk_0_3::Message &message)
{
	Q_D(JContact);
	JAccount *a = static_cast<JAccount*>(account());

	if(a->status() == Status::Offline)
		return false;
	qDebug("%s", Q_FUNC_INFO);
	//		if (!session())
	//			d_func()->account->messageHandler()->createSession(this);
	//		session()->sendMessage(message);

	//FIXME testing testing testing

	//	jreen::MessageSession *session = a->messageSessionManager()->session(d->jid,
	//																		 jreen::Message::Chat,
	//																		 true);
	//	if(!session) {
	//		session = a->messageSessionManager()->session(d->jid,
	//													  jreen::Message::Chat,
	//													  true);
	//		new JMessageReceiptFilter(this,session);
	//		session->registerMessageFilter(new JMessageReceiptFilter(this,session));
	//	}
	//	session->sendMessage(message.text(),message.property("subject").toString());

	jreen::Message msg(jreen::Message::Chat,
					   id(),
					   message.text(),
					   message.property("subject").toString());
	msg.addExtension(new jreen::Receipt(jreen::Receipt::Request));
	msg.setID(QString::number(message.id()));
	d->account->client()->send(msg);
	return true;
}

void JContact::setName(const QString &name)
{
	Q_D(JContact);
	if (d->name == name)
		return;
	setContactName(name);
	//		RosterManager *rosterManager = d->account->connection()->client()->rosterManager();
	//		RosterItem *item = rosterManager->getRosterItem(d->jid.toStdString());
	//		if (!item)
	//			return;
	//		item->setName(name.toStdString());
	//		rosterManager->synchronize();
}

void JContact::setContactName(const QString &name)
{
	Q_D(JContact);
	QString previous = d->name;
	d->name = name;
	emit nameChanged(name, previous);
}

QString JContact::name() const
{
	return d_func()->name;
}

void JContact::setTags(const QStringList &tags)
{
	Q_D(JContact);
	if (d->tags == tags)
		return;
	setContactTags(tags);
	//		RosterManager *rosterManager = d->account->connection()->client()->rosterManager();
	//		RosterItem *item = rosterManager->getRosterItem(JID(d->jid.toStdString()));
	//		if(!item)
	//			return;
	//		StringList stdGroups;
	//		foreach (QString group, d->tags) {
	//			stdGroups.push_back(group.toStdString());
	//		}
	//		item->setGroups(stdGroups);
	//		rosterManager->synchronize();
}

void JContact::setContactTags(const QStringList &tags)
{
	Q_D(JContact);
	QStringList previous = d->tags;
	d->tags = tags;
	emit tagsChanged(tags, previous);
}

QStringList JContact::tags() const
{
	return d_func()->tags;
}

bool JContact::isInList() const
{
	return d_func()->inList;
}

void JContact::setContactInList(bool inList)
{
	d_func()->inList = inList;
	emit inListChanged(inList);
}

void JContact::setInList(bool inList)
{
	Q_D(JContact);
	if (d->inList == inList)
		return;	
	if(inList)
		d->account->roster()->addContact(this);
	else
		d->account->roster()->removeContact(this);
}

void JContact::setContactSubscription(jreen::AbstractRosterItem::SubscriptionType subscription)
{
	d_func()->subscription = subscription;
}

jreen::AbstractRosterItem::SubscriptionType JContact::subscription() const
{
	return d_func()->subscription;
}

inline gloox::ChatStateType qutIM2gloox(qutim_sdk_0_3::ChatState state)
{
	switch (state) {
	case qutim_sdk_0_3::ChatStateActive:
		return gloox::ChatStateActive;
	case qutim_sdk_0_3::ChatStateInActive:
		return gloox::ChatStateInactive;
	case qutim_sdk_0_3::ChatStateGone:
		return gloox::ChatStateGone;
	case qutim_sdk_0_3::ChatStateComposing:
		return gloox::ChatStateComposing;
	case qutim_sdk_0_3::ChatStatePaused:
		return gloox::ChatStatePaused;
	default:
		return gloox::ChatStateInvalid;
	}
}

bool JContact::event(QEvent *ev)
{
	Q_D(JContact);
	if (ev->type() == ChatStateEvent::eventType()) {
		ChatStateEvent *chatEvent = static_cast<ChatStateEvent *>(ev);
		jreen::ChatState::State state = static_cast<jreen::ChatState::State>(chatEvent->chatState());

		jreen::Message msg(jreen::Message::Chat,
						   d->jid);
		msg.addExtension(new jreen::ChatState(state));
		d->account->client()->send(msg);
		return true;
	} else if (ev->type() == ToolTipEvent::eventType()) {
		Q_D(JContact);
		ToolTipEvent *event = static_cast<ToolTipEvent*>(ev);

		//TODO move to public method
		LocalizedString subscriptionStr;
		switch(d->subscription) {
		case jreen::AbstractRosterItem::None:
			subscriptionStr = QT_TRANSLATE_NOOP("Jabber","None");
			break;
		case jreen::AbstractRosterItem::From:
			subscriptionStr = QT_TRANSLATE_NOOP("Jabber","From");
			break;
		case jreen::AbstractRosterItem::To:
			subscriptionStr = QT_TRANSLATE_NOOP("Jabber","To");
			break;
		case jreen::AbstractRosterItem::Both:
			subscriptionStr = QT_TRANSLATE_NOOP("Jabber","Both");
			break;
		case jreen::AbstractRosterItem::Remove:
			subscriptionStr = QT_TRANSLATE_NOOP("Jabber","Remove");
			break;
		}

		event->addField(QT_TRANSLATE_NOOP("Jabber","Subscription"),
						subscriptionStr
						);

		foreach (const QString &id, d->resources.keys()) {
			JContactResource *resource = d->resources.value(id);
			ToolTipEvent resourceEvent(false);
			qApp->sendEvent(resource, &resourceEvent);
			event->addHtml("<hr>" + resourceEvent.html(), 9);
		}
	} else if (ev->type() == InfoRequestCheckSupportEvent::eventType()) {
		Status::Type status = account()->status().type();
		if (status >= Status::Online && status <= Status::Invisible) {
			InfoRequestCheckSupportEvent *event = static_cast<InfoRequestCheckSupportEvent*>(ev);
			event->setSupportType(InfoRequestCheckSupportEvent::Read);
			event->accept();
		} else {
			ev->ignore();
		}
	} else if (ev->type() == InfoRequestEvent::eventType()) {
		Q_D(JContact);
		InfoRequestEvent *event = static_cast<InfoRequestEvent*>(ev);
		if(!d->account->vCardManager()->containsRequest(d->jid))
			event->setRequest(new JInfoRequest(d->account->vCardManager(),
											   d->jid));
		event->accept();
	} else if(ev->type() == Authorization::Request::eventType()) {
		debug() << "Handle auth request";
		Authorization::Request *request = static_cast<Authorization::Request*>(ev);
		d->account->roster()->requestSubscription(d->jid,
												  request->body());
		return true;
	} else if(ev->type() == Authorization::Reply::eventType()) {
		debug() << "handle auth reply";
		Authorization::Reply *reply = static_cast<Authorization::Reply*>(ev);
		//FIXME may be moved to the JRoster?
		bool answer = (reply->replyType() == Authorization::Reply::Accept);
		jreen::Presence presence(answer ? jreen::Presence::Subscribed
										: jreen::Presence::Unsubscribed,
								 d->jid,
								 reply->body()
								 );
		d->account->client()->send(presence);
		d->account->roster()->synchronize();
		return true;
	}
	return Contact::event(ev);
}

void JContact::requestSubscription()
{
	Authorization::Request *request = new Authorization::Request(this);
	qApp->postEvent(Authorization::service(),request);
}

void JContact::removeSubscription()
{
	d_func()->account->roster()->removeSubscription(this);
}

bool JContact::hasResource(const QString &resource)
{
	return d_func()->resources.contains(resource);
}

void JContact::addResource(const QString &resource)
{
	JContactResource *res = new JContactResource(this, resource);
	connect(res, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
			SLOT(resourceStatusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)));
	d_func()->resources.insert(resource, res);
	emit lowerUnitAdded(res);
}

void JContact::setStatus(const jreen::Presence presence)
{
	QString resource = presence.from().resource();
	jreen::Presence::Type type = presence.subtype();

	Q_D(JContact);
	Status oldStatus = status();
	if (type == jreen::Presence::Unavailable && resource.isEmpty()) {
		qDeleteAll(d->resources);
		d->resources.clear();
		d->currentResources.clear();
	} else if (resource.isEmpty()) {
		return;
	} else if (type == jreen::Presence::Unavailable) {
		if (d->resources.contains(resource))
			removeResource(resource);
	} else {
		if (!d->resources.contains(resource))
			addResource(resource);
		d->resources.value(resource)->setStatus(presence);
		fillMaxResource();
	}
	Status newStatus = status();
	//		debug() << oldStatus.type() << newStatus.type();
	if(oldStatus.type() != newStatus.type())
		emit statusChanged(newStatus, oldStatus);
}

void JContact::setStatus(const QString &, Presence::PresenceType , int , const QString &)
{
}

void JContact::removeResource(const QString &resource)
{
	delete d_func()->resources.take(resource);
	fillMaxResource();
}

Status JContact::status() const
{
	Q_D(const JContact);
	Status status = d->currentResources.isEmpty() ?
				Status::instance(Status::Offline, "jabber") :
				d->resources.value(d->currentResources.first())->status();
	QHashIterator<QString, QVariantHash> itr(d->extInfo);
	while (itr.hasNext()) {
		itr.next();
		status.setExtendedInfo(itr.key(), itr.value());
	}
	return status;
}

void JContact::fillMaxResource()
{
	Q_D(JContact);
	d->currentResources.clear();
	foreach (QString resource, d->resources.keys()) {
		if (d->currentResources.isEmpty()) {
			d->currentResources << resource;
		} else {
			int prevPriority = d->resources.value(d->currentResources.first())->priority();
			if (d->resources.value(resource)->priority() > prevPriority) {
				d->currentResources.clear();
				d->currentResources << resource;
			} else if (d->resources.value(resource)->priority() == prevPriority) {
				d->currentResources << resource;
			}
		}
	}
}

QList<JContactResource *> JContact::resources()
{
	return d_func()->resources.values();
}

JContactResource *JContact::resource(const QString &key)
{
	return d_func()->resources.value(key);
}

ChatUnitList JContact::lowerUnits()
{
	ChatUnitList list;
	foreach(ChatUnit *unit, d_func()->resources)
		list << unit;
	return list;
}

QString JContact::avatar() const
{
	return d_func()->avatar;
}

QString JContact::avatarHash() const
{
	return d_func()->hash.toString();
}

void JContact::setAvatar(const QString &hex)
{
	Q_D(JContact);
	if (d->avatar == hex)
		return;
	d->avatar = d->account->getAvatarPath() % QLatin1Char('/') % hex;
	int pos = d->avatar.lastIndexOf('/') + 1;
	int length = d->avatar.length() - pos;
	d->hash = QStringRef(&d->avatar, pos, length);
	emit avatarChanged(d->avatar);
}

void JContact::setExtendedInfo(const QString &name, const QVariantHash &extStatus)
{
	Status current = status();
	d_func()->extInfo.insert(name, extStatus);
	emit statusChanged(status(), current);
}

void JContact::removeExtendedInfo(const QString &name)
{
	Status current = status();
	d_func()->extInfo.remove(name);
	emit statusChanged(status(), current);
}

void JContact::resourceStatusChanged(const Status &current, const Status &previous)
{
	Q_D(JContact);
	if (d->currentResources.isEmpty())
		return;
	if (d->resources.value(d->currentResources.first()) == sender())
		emit statusChanged(current, previous);
}

}

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
#include "jcontact.h"
#include "jcontactresource.h"
#include "../jaccount.h"
#include "../../jprotocol.h"
#include <QStringBuilder>
#include "qutim/tooltip.h"
#include "qutim/extensionicon.h"
#include <qutim/debug.h>
#include <qutim/message.h>
#include <qutim/rosterstorage.h>
#include "jmessagesession.h"
#include "jmessagehandler.h"
#include <qutim/metacontact.h>
#include <qutim/authorizationdialog.h>
#include <qutim/notification.h>
#include <QApplication>
//Jreen
#include <jreen/presence.h>
#include <jreen/client.h>
#include <jreen/chatstate.h>
#include "jroster.h"
#include "../jpgpsupport.h"

namespace Jabber
{

class JContactPrivate
{
public:
	JContactPrivate() : inList(false), encrypted(true) {}
	JAccount *account;
	QHash<QString, JContactResource *> resources;
	QStringList currentResources;
	QStringList tags;
	QString name;
	QString jid;
	bool inList;
	bool encrypted;
	QString avatar;
	QStringRef hash;
	QHash<QString, QVariantHash> extInfo;
	Jreen::RosterItem::SubscriptionType subscription;
	Status status;
	QString pgpKey;
};

JContact::JContact(const QString &jid, JAccount *account) : Contact(account), d_ptr(new JContactPrivate)
{
	Q_D(JContact);
	d->account = account;
	d->jid = jid;
	d->status = Status::instance(Status::Offline, "jabber");
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
	if(d->account->status() == Status::Offline || d->account->status() == Status::Connecting)
		return false;
	debug() << Q_FUNC_INFO;

	d->account->messageSessionManager()->sendMessage(this, message);
	return true;
}

void JContact::setName(const QString &name)
{
	Q_D(JContact);
	if (d->name == name)
		return;
	setContactName(name);
	d->account->roster()->setName(this,name);
}

void JContact::setContactName(const QString &name)
{
	Q_D(JContact);
	if (d->name == name)
		return;
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
	// wtf?
//	setContactTags(tags);
	d->account->roster()->setGroups(this,tags);
}

void JContact::setContactTags(const QStringList &tags)
{
	Q_D(JContact);
	if (d->tags == tags)
		return;
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
	Q_D(JContact);
	if (d->inList == inList)
		return;
	d->inList = inList;
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

void JContact::setContactSubscription(Jreen::RosterItem::SubscriptionType subscription)
{
	d_func()->subscription = subscription;
	emit subscriptionChanged(subscription);
}

Jreen::RosterItem::SubscriptionType JContact::subscription() const
{
	return d_func()->subscription;
}

void JContact::setEncrypted(bool encrypted)
{
	d_func()->encrypted = encrypted;
}

bool JContact::isEncrypted() const
{
	return d_func()->encrypted;
}

bool JContact::event(QEvent *ev)
{
	Q_D(JContact);
	if (ev->type() == ChatStateEvent::eventType()) {
		ChatStateEvent *chatEvent = static_cast<ChatStateEvent *>(ev);
		Jreen::ChatState::State state = static_cast<Jreen::ChatState::State>(chatEvent->chatState());

		Jreen::Message msg(Jreen::Message::Chat,
						   d->jid);
		msg.addExtension(new Jreen::ChatState(state));
		d->account->messageSessionManager()->send(msg);
		return true;
	} else if (ev->type() == ToolTipEvent::eventType()) {
		Q_D(JContact);
		ToolTipEvent *event = static_cast<ToolTipEvent*>(ev);

		//TODO move to public method
		LocalizedString subscriptionStr;
		switch(d->subscription) {
		case Jreen::RosterItem::None:
			subscriptionStr = QT_TRANSLATE_NOOP("Jabber","None");
			break;
		case Jreen::RosterItem::From:
			subscriptionStr = QT_TRANSLATE_NOOP("Jabber","From");
			break;
		case Jreen::RosterItem::To:
			subscriptionStr = QT_TRANSLATE_NOOP("Jabber","To");
			break;
		case Jreen::RosterItem::Both:
			subscriptionStr = QT_TRANSLATE_NOOP("Jabber","Both");
			break;
		case Jreen::RosterItem::Remove:
			subscriptionStr = QT_TRANSLATE_NOOP("Jabber","Remove");
			break;
		default:
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
		Jreen::Presence presence(answer ? Jreen::Presence::Subscribed
										: Jreen::Presence::Unsubscribed,
								 d->jid,
								 reply->body()
								 );
		d->account->client()->send(presence);
		d->account->roster()->synchronize();
		return true;
	} else if (ev->type() == MetaContactChangeEvent::eventType()) {
		MetaContactChangeEvent *event = static_cast<MetaContactChangeEvent*>(ev);
		QString id = event->newMetaContact() ? event->newMetaContact()->id() : QString();
		d->account->roster()->handleChange(this, id);
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
	connect(res,SIGNAL(chatStateChanged(qutim_sdk_0_3::ChatState,qutim_sdk_0_3::ChatState)),
			this,SIGNAL(chatStateChanged(qutim_sdk_0_3::ChatState,qutim_sdk_0_3::ChatState)));
	d_func()->resources.insert(resource, res);
	emit lowerUnitAdded(res);
}

void JContact::setStatus(const Jreen::Presence presence)
{
	Jreen::Error::Ptr error = presence.error();
	QString resource = presence.from().resource();
	Jreen::Presence::Type type = presence.subtype();

	Q_D(JContact);
	Status oldStatus = status();
	if ((type == Jreen::Presence::Unavailable || error) && resource.isEmpty()) {
		qDeleteAll(d->resources);
		d->resources.clear();
		d->currentResources.clear();
		d->status = JStatus::presenceToStatus(Jreen::Presence::Unavailable);
	} else if (resource.isEmpty()) {
		d->status = JStatus::presenceToStatus(error ? Jreen::Presence::Unavailable : presence.subtype());
		d->status.setText(presence.status());
	} else if (error || type == Jreen::Presence::Unavailable) {
		if (d->resources.contains(resource))
			removeResource(resource);
	} else {
		if (!d->resources.contains(resource))
			addResource(resource);
		// If signals of the resource would not be blocked,
		// statusChanged() signal would be emitted two times:
		// from resourceStatusChanged() and from this method.
		JContactResource *contactResource = d->resources.value(resource);
		contactResource->blockSignals(true);
		contactResource->setStatus(presence);
		contactResource->blockSignals(false);
		fillMaxResource();
		JPGPSupport::instance()->verifyPGPSigning(contactResource);
	}
	recalcStatus();
	if (oldStatus.type() != d->status.type()) {
		NotificationRequest request(this, d->status, oldStatus);
		request.send();
		emit statusChanged(d->status, oldStatus);
	}
}

void JContact::removeResource(const QString &resource)
{
	Q_D(JContact);
	delete d->resources.take(resource);
	fillMaxResource();
	if (d->resources.isEmpty()) {
		Status oldStatus = d->status;
		d->status = JStatus::presenceToStatus(Jreen::Presence::Unavailable);
		d->status.setExtendedInfos(oldStatus.extendedInfos());
		d->status.removeExtendedInfo(QLatin1String("client"));
		return;
	}
}

Status JContact::status() const
{
	return d_func()->status;
}

void JContact::recalcStatus()
{
	Q_D(JContact);
	Status status = d->currentResources.isEmpty() ?
				d->status :
				d->resources.value(d->currentResources.first())->status();
	QHashIterator<QString, QVariantHash> itr(d->extInfo);
	while (itr.hasNext()) {
		itr.next();
		status.setExtendedInfo(itr.key(), itr.value());
	}
	d->status = status;
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
	if (d->inList) {
		RosterStorage::instance()->updateContact(this, d->account->roster()->version());
	}
}

void JContact::setExtendedInfo(const QString &name, const QVariantHash &extStatus)
{
	Status current = status();
	d_func()->extInfo.insert(name, extStatus);
	recalcStatus();
	emit statusChanged(status(), current);
}

void JContact::removeExtendedInfo(const QString &name)
{
	Status current = status();
	d_func()->extInfo.remove(name);
	recalcStatus();
	emit statusChanged(status(), current);
}

QString JContact::pgpKeyId() const
{
	return d_func()->pgpKey;
}

void JContact::setPGPKeyId(QString pgpKey)
{
	Q_D(JContact);
	d->pgpKey = pgpKey;
	if (d->inList && !d->account->roster()->ignoreChanges())
		RosterStorage::instance()->updateContact(this);
	emit pgpKeyChangedId(pgpKey);
}

void JContact::resourceStatusChanged(const Status &current, const Status &previous)
{
	Q_D(JContact);
	if (d->currentResources.isEmpty())
		return;
	if (d->resources.value(d->currentResources.first()) == sender()) {
		recalcStatus();
		if (current.type() != previous.type() || current.text() != previous.text()) {
			NotificationRequest request(this, current, previous);
			request.send();
		}
		emit statusChanged(current, previous);
	}
}

}


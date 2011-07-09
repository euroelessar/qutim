/****************************************************************************
**
** qutIM instant messenger
**
** Copyright (c) 2011 Ruslan Nigmatullin <euroelessar@gmail.com>
**                    Alexey Prokhin <alexey.prokhin@yandex.ru>
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
** along with this program. If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#include "jvcardmanager.h"
#include "jinforequest.h"
#include "../muc/jmucuser.h"
#include "../roster/jcontact.h"
#include "../roster/jroster.h"
#include "../jaccount.h"
#include "../../jprotocol.h"
#include <QDir>
#include <qutim/debug.h>
#include <qutim/rosterstorage.h>
#include <jreen/vcard.h>
#include <jreen/vcardupdate.h>
#include <jreen/iq.h>
#include <jreen/client.h>
#include <QCryptographicHash>

namespace Jabber
{

class JVCardManagerPrivate
{
public:
	JAccount *account;
	//VCardManager *manager;
	QHash<QString, QWeakPointer<JInfoRequest> > contacts;
	QSet<ChatUnit*> observedUnits;
};

static bool isStatusOnline(const Status &status)
{
	Status::Type type = status.type();
	return type != Status::Offline && type != Status::Connecting;
}

JVCardManager::JVCardManager(JAccount *account)
	: QObject(account), d_ptr(new JVCardManagerPrivate)
{
	Q_D(JVCardManager);
	d->account = account;
	connect(account->client(),SIGNAL(iqReceived(Jreen::IQ)),
			SLOT(handleIQ(Jreen::IQ)));
	connect(account, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
			SLOT(onAccountStatusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)));
}

JVCardManager::~JVCardManager()
{

}

void JVCardManager::fetchVCard(const QString &contact, JInfoRequest *request)
{
	Q_D(JVCardManager);	
	if (!d->contacts.contains(contact)) {
		debug() << "fetch vcard";
		d->contacts.insert(contact, request);
		//fetch iq
		Jreen::IQ iq(Jreen::IQ::Get,contact);
		iq.addExtension(new Jreen::VCard());
		d->account->client()->send(iq,this,SLOT(onIqReceived(Jreen::IQ,int)),0);
	}
}

void JVCardManager::storeVCard(const Jreen::VCard::Ptr &vcard, QObject *receiver, const char *slot)
{
	Q_D(JVCardManager);
	Jreen::IQ iq(Jreen::IQ::Set, JID());
	iq.addExtension(vcard);
	d->account->client()->send(iq,receiver,slot,0);
}

void JVCardManager::handleVCard(const Jreen::VCard::Ptr vcard, const QString &id)
{
	Q_D(JVCardManager);
	QString avatarHash;
	const Jreen::VCard::Photo &photo = vcard->photo();
	if (!photo.data().isEmpty()) {
		avatarHash = QCryptographicHash::hash(photo.data(), QCryptographicHash::Sha1).toHex();
		QDir dir(d->account->getAvatarPath());
		if (!dir.exists())
			dir.mkpath(dir.absolutePath());
		QFile file(dir.absoluteFilePath(avatarHash));
		if (file.open(QIODevice::WriteOnly)) {
			file.write(photo.data());
			file.close();
		}
	}
	if (d->account->id() == id) {
		QString nick = vcard->nickname();
		if(nick.isEmpty())
			nick = vcard->formattedName();
		if(nick.isEmpty())
			nick = d->account->id();
		if (d->account->name() != nick)
			d->account->setNick(nick);
		Jreen::Presence presence = d->account->client()->presence();
		Jreen::VCardUpdate::Ptr update = presence.payload<Jreen::VCardUpdate>();
		if (update->photoHash() != avatarHash) {
			d->account->config("general").setValue("photoHash", avatarHash);
			d->account->setAvatarHex(avatarHash);
			d->account->client()->send(presence);
		}
	} else {
		ChatUnit *unit = d->account->getUnit(id);
		if (JContact *contact = qobject_cast<JContact *>(unit)) {
			contact->setAvatar(avatarHash);
			if (contact->isInList())
				RosterStorage::instance()->updateContact(contact, account()->roster()->version());
		} else if (JMUCUser *contact = qobject_cast<JMUCUser *>(unit)) {
			contact->setAvatar(avatarHash);
		}
	}
	debug() << "fetched...";
	if (JInfoRequest *request = d->contacts.take(id).data())
		request->setFetchedVCard(vcard);
}

bool JVCardManager::containsRequest(const QString &contact)
{
	return d_func()->contacts.contains(contact);
}


JAccount * JVCardManager::account() const
{
	return d_func()->account;
}

JVCardManager::SupportLevel JVCardManager::supportLevel(QObject *object)
{
	Q_D(JVCardManager);
	if (d->account == object) {
		return isStatusOnline(d->account->status()) ? ReadWrite : Unavailable;
	} else if (ChatUnit *unit = qobject_cast<ChatUnit*>(object)) {
		Account *acc = unit->account();
		if (acc == d->account)
			return isStatusOnline(acc->status()) ? ReadOnly : Unavailable;
	}
	return NotSupported;
}

InfoRequest *JVCardManager::createrDataFormRequest(QObject *object)
{
	Q_D(JVCardManager);
	if (d->account == object) {
		return new JInfoRequest(this, d->account);
	} else if (ChatUnit *unit = qobject_cast<ChatUnit*>(object)) {
		if (unit->account() == d->account)
			return new JInfoRequest(this, unit);
	}
	return 0;
}

bool JVCardManager::startObserve(QObject *object)
{
	Q_D(JVCardManager);
	if (d->account == object) {
		return true;
	} else if (ChatUnit *unit = qobject_cast<ChatUnit*>(object)) {
		if (unit->account() == d->account) {
			d->observedUnits.insert(unit);
			return true;
		}
	}
	return false;
}

bool JVCardManager::stopObserve(QObject *object)
{
	Q_D(JVCardManager);
	if (d->account == object)
		return true;
	else if (ChatUnit *contact = qobject_cast<ChatUnit*>(object))
		return d->observedUnits.remove(contact) != 0;
	return false;
}

void JVCardManager::onAccountStatusChanged(const qutim_sdk_0_3::Status &status,
										   const qutim_sdk_0_3::Status &previous)
{
	Q_D(JVCardManager);
	bool isOnline = isStatusOnline(status);
	bool wasOnline = isStatusOnline(previous);
	bool supported;

	if (isOnline && !wasOnline)
		supported = true;
	else if (!isOnline && wasOnline)
		supported = false;
	else
		return;

	SupportLevel level = supported ? ReadWrite : Unavailable;
	setSupportLevel(d->account, level);

	level = supported ? ReadOnly : Unavailable;
	foreach (ChatUnit *unit, d->observedUnits)
		setSupportLevel(unit, level);

}

void JVCardManager::onIqReceived(const Jreen::IQ &iq, int)
{
	debug() << "vcard received";
	if(!iq.containsPayload<Jreen::VCard>())
		return;
	iq.accept();
	handleVCard(iq.payload<Jreen::VCard>(), iq.from().full());
}

} //namespace jabber



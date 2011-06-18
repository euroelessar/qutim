#include "jvcardmanager.h"
#include "jinforequest.h"
#include "../muc/jmucuser.h"
#include "../roster/jcontact.h"
#include "../roster/jroster.h"
#include "../jaccount.h"
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
};

JVCardManager::JVCardManager(JAccount *account)
	: QObject(account), d_ptr(new JVCardManagerPrivate)
{
	Q_D(JVCardManager);
	d->account = account;
	connect(account->client(),SIGNAL(iqReceived(Jreen::IQ)),
			SLOT(handleIQ(Jreen::IQ)));
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

void JVCardManager::storeVCard(const Jreen::VCard::Ptr &vcard)
{
	Q_UNUSED(vcard);
	//Q_D(JVCardManager);
	//d->manager->storeVCard(vcard, this);
}

void JVCardManager::handleIQ(const Jreen::IQ &iq)
{
	Q_D(JVCardManager);
	debug() << "handle IQ";
	if(!iq.containsPayload<Jreen::VCard>())
		return;
	iq.accept();
	QString id = iq.from().full();
	QString avatarHash;
	const Jreen::VCard::Ptr vcard = iq.payload<Jreen::VCard>();
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
			update->setPhotoHash(avatarHash);
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

void JVCardManager::onIqReceived(const Jreen::IQ &iq, int)
{
	debug() << "vcard received";
	handleIQ(iq);
}

} //namespace jabber



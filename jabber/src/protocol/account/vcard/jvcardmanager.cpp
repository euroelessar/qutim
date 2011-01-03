#include "jvcardmanager.h"
#include "jinforequest.h"
#include "../muc/jmucuser.h"
#include "../roster/jcontact.h"
#include "../roster/jroster.h"
#include "../jaccount.h"
#include <QDir>
#include <qutim/debug.h>
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
	QHash<QString, JInfoRequest *> contacts;
};

JVCardManager::JVCardManager(JAccount *account)
	: QObject(account), d_ptr(new JVCardManagerPrivate)
{
	Q_D(JVCardManager);
	d->account = account;
	connect(account->client(),SIGNAL(newIQ(jreen::IQ)),
			SLOT(handleIQ(jreen::IQ)));
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
		jreen::IQ iq(jreen::IQ::Get,contact);
		iq.addExtension(new jreen::VCard());
		d->account->client()->send(iq,this,SLOT(onIqReceived(jreen::IQ,int)),0);
	}
}

void JVCardManager::storeVCard(jreen::VCard *vcard)
{
	//Q_D(JVCardManager);
	//d->manager->storeVCard(vcard, this);
}

void JVCardManager::handleIQ(const jreen::IQ &iq)
{
	Q_D(JVCardManager);
	debug() << "handle IQ";
	if(!iq.containsExtension<jreen::VCard>())
		return;
	iq.accept();
	QString id = iq.from().full();
	QString avatarHash;
	jreen::VCard *vcard = iq.findExtension<jreen::VCard>().data();
	const jreen::VCard::Photo &photo = vcard->photo();
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
		jreen::Presence presence = d->account->client()->presence();
		jreen::VCardUpdate::Ptr update = presence.findExtension<jreen::VCardUpdate>();
		if (update->photoHash() != avatarHash) {
			d->account->config("general").setValue("photoHash", avatarHash);
			update->setPhotoHash(avatarHash);
			d->account->client()->send(presence);
		}
	} else {
		ChatUnit *unit = d->account->getUnit(id);
		if (JContact *contact = qobject_cast<JContact *>(unit))
			contact->setAvatar(avatarHash);
		else if (JMUCUser *contact = qobject_cast<JMUCUser *>(unit))
			contact->setAvatar(avatarHash);
	}
	debug() << "fetched...";
	if (JInfoRequest *request = d->contacts.take(id))
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

void JVCardManager::onIqReceived(const jreen::IQ &iq, int)
{
	debug() << "vcard received";
	handleIQ(iq);
}

} //namespace jabber



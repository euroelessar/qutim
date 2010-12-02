#include "jvcardmanager.h"
#include "jinforequest.h"
#include "../muc/jmucuser.h"
#include "../roster/jcontact.h"
#include "../roster/jroster.h"
#include "../jaccount.h"
#include <QDir>
#include <gloox/client.h>
#include <gloox/vcardmanager.h>
#include <gloox/sha.h>
#include <qutim/debug.h>
#include <jreen/vcard.h>
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
}

JVCardManager::~JVCardManager()
{

}

void JVCardManager::fetchVCard(const QString &contact, JInfoRequest *request)
{
	Q_D(JVCardManager);
	debug() << "fetch vcard";
	if (!d->contacts.contains(contact)) {
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

void JVCardManager::handleVCard(const jreen::JID &jid,QSharedPointer<jreen::VCard> fetchedVCard)
{
	Q_D(JVCardManager);
	QString id = jid.full();
	QString avatar;
	jreen::VCard *vcard = fetchedVCard.isNull() ? new jreen::VCard() : fetchedVCard.data();
	const jreen::VCard::Photo &photo = vcard->photo();
	if (!photo.binval.isEmpty()) {
		avatar = QCryptographicHash::hash(photo.binval.toAscii(),QCryptographicHash::Sha1);
		QDir dir(d->account->getAvatarPath());
		if (!dir.exists())
			dir.mkpath(dir.absolutePath());
		QFile file(dir.absoluteFilePath(avatar));
		if (file.open(QIODevice::WriteOnly)) {
			file.write(photo.binval.toAscii());
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
		//d->account->connection()->setAvatar(avatar);
	} else {
		ChatUnit *unit = d->account->getUnit(id);
		if (JContact *contact = qobject_cast<JContact *>(unit))
			contact->setAvatar(avatar);
		else if (JMUCUser *contact = qobject_cast<JMUCUser *>(unit))
			contact->setAvatar(avatar);
	}
	debug() << "fetched...";
	if (JInfoRequest *request = d->contacts.take(id))
		request->setFetchedVCard(vcard);
}

void JVCardManager::onIqReceived(const jreen::IQ &iq, int)
{
	debug() << "vcard received";
	handleVCard(iq.from(),iq.findExtension<jreen::VCard>());
}

//void JVCardManager::handleVCardResult(VCardContext context, const JID &jid, StanzaError se)
//{
//	//if (context == StoreVCard && se == StanzaErrorUndefined); wtf?
//}

}

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

namespace Jabber
{
class JVCardManagerPrivate
{
public:
	JAccount *account;
	VCardManager *manager;
	QHash<QString, JInfoRequest *> contacts;
};

JVCardManager::JVCardManager(JAccount *account, Client *client, QObject *parent)
	: QObject(parent), d_ptr(new JVCardManagerPrivate)
{
	Q_D(JVCardManager);
	d->account = account;
	d->manager = new VCardManager(client);
}

JVCardManager::~JVCardManager()
{

}

VCardManager *JVCardManager::manager()
{
	return d_ptr->manager;
}

void JVCardManager::fetchVCard(const QString &contact, JInfoRequest *request)
{
	Q_D(JVCardManager);
	if (!d->contacts.contains(contact)) {
		d->contacts.insert(contact, request);
		d->manager->fetchVCard(contact.toStdString(), this);
	}
}

void JVCardManager::storeVCard(VCard *vcard)
{
	Q_D(JVCardManager);
	d->manager->storeVCard(vcard, this);
}

void JVCardManager::handleVCard(const JID &jid, const VCard *fetchedVCard)
{
	Q_D(JVCardManager);
	QString id = QString::fromStdString(jid.full());
	QString avatar;
	VCard *vcard = (!fetchedVCard) ? new VCard() : static_cast<VCard*>(fetchedVCard->clone());
	const VCard::Photo &photo = vcard->photo();
	if (!photo.binval.empty()) {
		SHA sha;
		sha.feed(photo.binval);
		sha.finalize();
		avatar = QString::fromStdString(sha.hex());
		QDir dir(d->account->getAvatarPath());
		if (!dir.exists())
			dir.mkpath(dir.absolutePath());
		QFile file(dir.absoluteFilePath(avatar));
		if (file.open(QIODevice::WriteOnly)) {
			file.write(photo.binval.c_str(), photo.binval.length());
			file.close();
		}
	}
	if (d->account->id() == id) {
		QString nick = QString::fromStdString(vcard->nickname());
		if(nick.isEmpty())
			nick = QString::fromStdString(vcard->formattedname());
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
	else
		delete vcard;
}

void JVCardManager::handleVCardResult(VCardContext context, const JID &jid, StanzaError se)
{
	//if (context == StoreVCard && se == StanzaErrorUndefined); wtf?
}
}

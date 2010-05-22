#include "jroster.h"
#include "../jaccount.h"
#include "../../jprotocol.h"
#include "jcontact.h"
#include "jcontactresource.h"
#include "../vcard/jvcardmanager.h"
#include <QFile>
#include <gloox/vcardupdate.h>
#include <QFile>
#include <QStringBuilder>

namespace Jabber
{
	struct JRosterPrivate
	{
		JAccount *account;
		RosterManager *rosterManager;
		QHash<QString, JContact*> contacts;
		bool avatarsAutoLoad;
	};

	JRoster::JRoster(JAccount *account) : p(new JRosterPrivate)
	{
		loadSettings();
		p->account = account;
		p->rosterManager = p->account->connection()->client()->rosterManager();
		p->rosterManager->registerRosterListener(this, false);
		p->account->connection()->client()->registerPresenceHandler(this);
	}

	JRoster::~JRoster()
	{
	}

	ChatUnit *JRoster::contact(const QString &id, bool create)
	{
		JID jid(id.toStdString());
		QString bare = id.contains('/') ? QString::fromStdString(jid.bare()) : id;
		QString resourceId = id == bare ? QString() : QString::fromStdString(jid.resource());
		JContact *contact = p->contacts.value(bare);
		if (!resourceId.isEmpty()) {
			if (!contact) {
				if (create) {
					// TODO: implement logic
					return 0;
				} else {
					return 0;
				}
			}
			if (JContactResource *resource = contact->resource(resourceId))
				return resource;
			if (create) {
				// TODO: implement logic
				return 0;
			}
		} else if (contact) {
			return contact;
		} else if (create) {
			// TODO: implement logic
			return 0;
		}
		return 0;
	}

	void JRoster::handleItemAdded(const JID &jid)
	{
		/*QString key(QString::fromStdString(jid.bare()));
		QString resource(QString::fromStdString(jid.resource()));
		if (!p->contacts.contains(key)) {
			//p->contacts.value(key)->addResource(resource);
		//} else {
			if (key != p->account->jid()) {
				JContact *contact = new JContact(p->account);
				contact->setName(QString::fromStdString(jid.username()));
				RosterItem *item = p->rosterManager->getRosterItem(jid);
				QSet<QString> tags;
				StringList groups = item->groups();
				StringList::const_iterator group = groups.begin();
				for(; group != groups.end(); ++group)
					tags.insert(QString::fromStdString(*group));
				contact->setTags(tags);
				//contact->addResource(key);
				contact->addToList();
				ContactList::instance()->addContact(contact);
				p->contacts.insert(key, contact);
			}
		}*/
	}

	void JRoster::handleItemSubscribed(const JID &jid)
	{
	}

	void JRoster::handleItemRemoved(const JID &jid)
	{
		/*QString key(QString::fromStdString(jid.bare()));
		QString resource(QString::fromStdString(jid.resource()));
		if (p->contacts.contains(key)) {
			if (!resource.isEmpty()) {
				p->contacts.value(key)->removeResource(resource);
				if (p->contacts.value(key)->resources().isEmpty())
					delete p->contacts.take(key);
			} else {
				delete p->contacts.take(key);
			}
		}*/
	}

	void JRoster::handleItemUpdated(const JID &jid)
	{
		/*RosterItem *item = p->rosterManager->getRosterItem(jid);
		QString key(QString::fromStdString(jid.bare()));
		if (p->contacts.contains(key)) {
			JContact *contact = p->contacts.value(key);
			contact->setName(QString::fromStdString(item->name()));
			QSet<QString> tags;
			StringList groups = item->groups();
			StringList::const_iterator group = groups.begin();
			for(; group != groups.end(); ++group)
				tags.insert(QString::fromStdString(*group));
			contact->setTags(tags);
		}*/
	}

	void JRoster::handleItemUnsubscribed(const JID &jid)
	{
	}

	void JRoster::handleRoster(const Roster &roster)
	{
		std::map<const std::string, RosterItem *>::const_iterator items = roster.begin();
		for(; items != roster.end(); ++items) {
			RosterItem *item = items->second;
			QString jid(QString::fromStdString(item->jidJID().bare()));
			if (jid == p->account->id()) {
				continue;
			} else if (!p->contacts.contains(jid)) {
				JContact *contact = new JContact(jid, p->account);
				contact->setContactName(QString::fromStdString(item->name()));
				QSet<QString> tags;
				StringList groups = item->groups();
				StringList::const_iterator group = groups.begin();
				for(; group != groups.end(); ++group)
					tags.insert(QString::fromStdString(*group));
				contact->setContactTags(tags);
				contact->setContactInList(true);
				ContactList::instance()->addContact(contact);
				p->contacts.insert(jid, contact);
			}
		}
	}

	void JRoster::handleRosterPresence(const RosterItem &item,
			const std::string &resource, Presence::PresenceType presence,
			const std::string &msg)
	{
	}

	void JRoster::handlePresence(const Presence &presence)
	{
		QString jid(QString::fromStdString(presence.from().bare()));
		QString resource(QString::fromStdString(presence.from().resource()));
		if (presence.from().bare() == p->account->client()->jid().bare())
			 return;
		if (!p->contacts.contains(jid)) {
			JContact *contact = new JContact(jid, p->account);
			contact->setContactName(QString::fromStdString(presence.from().username()));
			contact->setContactInList(false);
			ContactList::instance()->addContact(contact);
			p->contacts.insert(jid, contact);
		}
		if (!resource.isEmpty())
			p->contacts.value(jid)->setStatus(resource, presence.presence(), presence.priority(),
					QString::fromStdString(presence.status()));
		if (presence.presence() != Presence::Unavailable && !presence.error()) {
			const VCardUpdate *vcard = presence.findExtension<VCardUpdate>(ExtVCardUpdate);
			if(vcard) {
				QString hash = QString::fromStdString(vcard->hash());
				JContact *contact = p->contacts.value(jid);
				if (contact->avatarHash() != hash) {
					if(hash.isEmpty() || QFile(p->account->getAvatarPath()%QLatin1Char('/')%hash).exists())
						contact->setAvatar(hash);
					else if (p->avatarsAutoLoad)
						p->account->connection()->vCardManager()->fetchVCard(jid);
				}
			}
		}
	}

	void JRoster::handleSelfPresence(const RosterItem &item, const std::string &resource,
			Presence::PresenceType presence, const std::string &msg)
	{
	}

	bool JRoster::handleSubscriptionRequest(const JID &jid, const std::string &msg)
	{
		return false;
	}

	bool JRoster::handleUnsubscriptionRequest(const JID &jid, const std::string &msg)
	{
		return true;
	}

	void JRoster::handleNonrosterPresence(const Presence &presence)
	{
	}

	void JRoster::handleRosterError(const IQ &iq)
	{
	}

	void JRoster::loadSettings()
	{
		p->avatarsAutoLoad = JProtocol::instance()->config("general").value("getavatars", true);
	}

	void JRoster::setOffline()
	{
		foreach (JContact *contact, p->contacts)
			contact->setStatus("", Presence::Unavailable, 0);
	}
}

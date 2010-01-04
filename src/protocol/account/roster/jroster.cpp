#include "jroster.h"
#include "jcontact_p.h"
#include <qutim/debug.h>

namespace Jabber
{
	struct JRosterPrivate
	{
		JAccount *account;
		RosterManager *rosterManager;
		QHash<QString, JContact*> contacts;
	};

	JRoster::JRoster(JAccount *account) : p(new JRosterPrivate)
	{
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
		qDeleteAllLater(p->contacts);
		p->contacts.clear();
		std::map<const std::string, RosterItem *>::const_iterator items = roster.begin();
		for(; items != roster.end(); ++items) {
			std::string key = items->first;
			QString jid(QString::fromStdString(JID(key).bare()));
			if (jid == p->account->jid()) {
					continue;
			} else if (!p->contacts.contains(jid)) {
				JContact *contact = new JContact(jid, p->account);
				JContactPrivate *c_d = contact->d_func();
				RosterItem *item = items->second;
				c_d->name = QString::fromStdString(item->name());
				QSet<QString> tags;
				StringList groups = item->groups();
				StringList::const_iterator group = groups.begin();
				for(; group != groups.end(); ++group)
					tags.insert(QString::fromStdString(*group));
				c_d->tags = tags;
				//QMap<std::string, Resource *> resources(item->resources());
				//foreach (std::string key, resources.keys())
				/*std::map<std::string, Resource *>::const_iterator resource = item->resources().begin();
				for(; resource != item->resources().end(); ++resource) {
					debug() << QString::fromStdString(resource->first);
					contact->addResource(QString::fromStdString(resource->first));
				}*/
				c_d->inList = true;
				debug() << contact->id() << contact->name() << contact->tags();
				ContactList::instance()->addContact(contact);
				p->contacts.insert(jid, contact);
				//debug() << contact->name() << contact->tags();
			}
		}
		debug() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~roster";
	}

	void JRoster::handleRosterPresence(const RosterItem &item,
			const std::string &resource, Presence::PresenceType presence,
			const std::string &msg)
	{
		//debug() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~roster presence";
	}

	void JRoster::handlePresence(const Presence &presence)
	{
		debug() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~presence";
		QString jid(QString::fromStdString(presence.from().bare()));
		QString resource(QString::fromStdString(presence.from().resource()));
		 if (jid == p->account->jid())
			 return;
		debug() << QString::fromStdString(presence.from().full()) << jid << resource;
		if (!p->contacts.contains(jid)) {
			JContact *contact = new JContact(jid, p->account);
			JContactPrivate *c_d = contact->d_func();
			c_d->name = QString::fromStdString(presence.from().username());
			RosterItem *item = p->rosterManager->getRosterItem(presence.from());
			QSet<QString> tags;
			StringList groups = item->groups();
			StringList::const_iterator group = groups.begin();
			for(; group != groups.end(); ++group)
				tags.insert(QString::fromStdString(*group));
			c_d->tags = tags;
			c_d->inList = false;
			debug() << contact->id() << contact->name() << contact->tags();
			ContactList::instance()->addContact(contact);
			p->contacts.insert(jid, contact);
		}
		if (!resource.isEmpty())
			p->contacts.value(jid)->setStatus(resource,
					presence.presence(), presence.priority());
	}

	void JRoster::handleSelfPresence(const RosterItem &item, const std::string &resource,
			Presence::PresenceType presence, const std::string &msg)
	{
	}

	bool JRoster::handleSubscriptionRequest(const JID &jid, const std::string &msg)
	{
		debug() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~subscription request";
		return false;
	}

	bool JRoster::handleUnsubscriptionRequest(const JID &jid, const std::string &msg)
	{
		debug() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~unsubscription request";
		return true;
	}

	void JRoster::handleNonrosterPresence(const Presence &presence)
	{
		//debug() << QString::fromStdString(presence.tag()->xml());
		debug() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~nonroster presence";
	}

	void JRoster::handleRosterError(const IQ &iq)
	{
		debug() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~roster error";
	}
}

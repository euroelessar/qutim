#include "jroster.h"
#include <qutim/debug.h>

namespace Jabber
{
	JRoster::JRoster(JAccount *account) : RosterManager(account->connection()->client())
	{
		account->connection()->client()->rosterManager()->registerRosterListener(this);
	}

	JRoster::~JRoster()
	{
	}

	void JRoster::handleItemAdded(const JID &jid)
	{
		debug() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~item added";
	}

	void JRoster::handleItemSubscribed(const JID &jid)
	{
		debug() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~item subscribed";
	}

	void JRoster::handleItemRemoved(const JID &jid)
	{
		debug() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~item removed";
	}

	void JRoster::handleItemUpdated(const JID &jid)
	{
		debug() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~item updated";
	}

	void JRoster::handleItemUnsubscribed(const JID &jid)
	{
		debug() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~item unsubscribed";
	}

	void JRoster::handleRoster(const Roster &roster)
	{
		debug() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~roster";
	}

	void JRoster::handleRosterPresence(const RosterItem &item,
			const std::string &resource, Presence::PresenceType presence,
			const std::string &msg)
	{
		debug() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~roster presence";
	}

	void JRoster::handleSelfPresence(const RosterItem &item, const std::string &resource,
			Presence::PresenceType presence, const std::string &msg)
	{
		debug() << QString::fromStdString(item.jid()) << QString::fromStdString(resource) << presence << QString::fromStdString(msg);
		debug() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~self presence";
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

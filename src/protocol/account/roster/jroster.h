#ifndef JROSTER_H
#define JROSTER_H

#include <gloox/rostermanager.h>
#include "../jaccount.h"

namespace Jabber
{
	using namespace gloox;

	class JAccount;

	class JRoster : public RosterManager, public RosterListener
	{
		public:
			JRoster(JAccount *account);
			~JRoster();
		protected:
			void handleItemAdded(const JID &jid);
			void handleItemSubscribed(const JID &jid);
			void handleItemRemoved(const JID &jid);
			void handleItemUpdated(const JID &jid);
			void handleItemUnsubscribed(const JID &jid);
			void handleRoster(const Roster &roster);
			void handleRosterPresence(const RosterItem &item,
					const std::string &resource, Presence::PresenceType presence,
					const std::string &msg);
			void handleSelfPresence(const RosterItem &item, const std::string &resource,
					Presence::PresenceType presence, const std::string &msg);
			bool handleSubscriptionRequest(const JID &jid, const std::string &msg);
			bool handleUnsubscriptionRequest(const JID &jid, const std::string &msg);
			void handleNonrosterPresence(const Presence &presence);
			void handleRosterError(const IQ &iq);
	};
}
#endif // JROSTER_H

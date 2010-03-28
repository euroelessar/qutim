#ifndef JROSTER_H
#define JROSTER_H

#include <qutim/contactlist.h>
#include <gloox/rostermanager.h>
#include <gloox/presencehandler.h>
#include "../jaccount.h"
#include "jcontact.h"

namespace Jabber
{
	using namespace qutim_sdk_0_3;
	using namespace gloox;

	class JAccount;
	struct JRosterPrivate;

	class JRoster : public QObject, public RosterListener, public PresenceHandler
	{
		Q_OBJECT
		public:
			JRoster(JAccount *account, const Config &config);
			~JRoster();
			ChatUnit *contact(const QString &jid, bool create = false);
			void setOffline();
		protected:
			void loadSettings();
			void handleItemAdded(const JID &jid);
			void handleItemSubscribed(const JID &jid);
			void handleItemRemoved(const JID &jid);
			void handleItemUpdated(const JID &jid);
			void handleItemUnsubscribed(const JID &jid);
			void handleRoster(const Roster &roster);
			void handleRosterPresence(const RosterItem &item,
					const std::string &resource, Presence::PresenceType presence,
					const std::string &msg);
			void handlePresence(const Presence &presence);
			void handleSelfPresence(const RosterItem &item, const std::string &resource,
					Presence::PresenceType presence, const std::string &msg);
			bool handleSubscriptionRequest(const JID &jid, const std::string &msg);
			bool handleUnsubscriptionRequest(const JID &jid, const std::string &msg);
			void handleNonrosterPresence(const Presence &presence);
			void handleRosterError(const IQ &iq);
		private:
			QScopedPointer<JRosterPrivate> p;
	};
}
#endif // JROSTER_H

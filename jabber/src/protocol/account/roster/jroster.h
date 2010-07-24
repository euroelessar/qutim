#ifndef JROSTER_H
#define JROSTER_H

#include <qutim/contact.h>
#include <qutim/configbase.h>
#include <gloox/rostermanager.h>
#include <gloox/presencehandler.h>
#include <gloox/subscriptionhandler.h>
#include <qutim/messagesession.h>
#include "metacontacts.h"

namespace Jabber
{
	using namespace qutim_sdk_0_3;
	using namespace gloox;

	class JAccount;
	class JContact;
	struct JRosterPrivate;

	class JRoster : 
			public QObject, public RosterListener, public PresenceHandler,
			public SubscriptionHandler, public MetaContactHandler
	{
		Q_OBJECT
		public:
			JRoster(JAccount *account);
			~JRoster();
			ChatUnit *contact(const QString &jid, bool create = false);
			void setOffline();
		protected:
			void loadSettings();
			void fillContact(JContact *contact, const RosterItem &item);
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
			void handleSubscription(const Subscription &subscription);
			void handleMetaContact(const MetaContactList &mcList);
			bool eventFilter(QObject *obj, QEvent *ev);
		private slots:
			void sendAuthResponse(bool answer);
			void onSessionCreated(qutim_sdk_0_3::ChatSession *session);
		private:
			QScopedPointer<JRosterPrivate> p;
	};
}
#endif // JROSTER_H

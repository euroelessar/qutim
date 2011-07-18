#ifndef JROSTER_H
#define JROSTER_H

#include <qutim/contact.h>
#include <qutim/configbase.h>
#include <qutim/messagesession.h>
#include "metacontacts.h"
//Jreen
#include <jreen/abstractroster.h>
#include <jreen/message.h>


namespace Jabber
{
using namespace qutim_sdk_0_3;
class JAccount;
class JContact;

class JRosterPrivate;
class JRoster : public Jreen::AbstractRoster
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(JRoster)
	Q_CLASSINFO("DebugName", "Jabber::Roster")
public:
	JRoster(JAccount *account);
	virtual ~JRoster();
	void loadFromStorage();
	ChatUnit *contact(const Jreen::JID &id, bool create = false);
	void addContact(const JContact *contact);
	void removeContact(const JContact *contact);
	void requestSubscription(const Jreen::JID &id, const QString &reason = QString());
	void removeSubscription(const JContact *contact);
	void setName(const JContact *contact, const QString &name);
	void setGroups(const JContact *contact, const QStringList &groups);
public slots:
	void loadSettings();
	void saveSettings();
protected:
	virtual void onItemAdded(QSharedPointer<Jreen::RosterItem> item);
	virtual void onItemUpdated(QSharedPointer<Jreen::RosterItem> item);
	virtual void onItemRemoved(const QString &jid);
	virtual void onLoaded(const QList<QSharedPointer<Jreen::RosterItem> > &items);
	void fillContact(JContact *contact, QSharedPointer<Jreen::RosterItem> item);
protected slots:
	void handleNewPresence(Jreen::Presence);
	void handleSubscription(Jreen::Presence subscribe); //TODO may be need a separated subscription manager?
	void handleAccountResourcePresence(const Jreen::Presence &precense);
	void onDisconnected();
	void onNewMessage(Jreen::Message message); //TODO move this method to JMessageManager
	void onContactDestroyed(QObject *obj);
private:
	JContact *createContact(const Jreen::JID &id);
	QScopedPointer<JRosterPrivate> d_ptr;
};


//dead code
//using namespace gloox;

//class JAccount;
//class JContact;
//struct JRosterPrivate;

//class JRoster :
//		public QObject, public RosterListener, public PresenceHandler,
//		public SubscriptionHandler, public MetaContactHandler
//{
//	Q_OBJECT
//public:
//	JRoster(JAccount *account);
//	~JRoster();
//	ChatUnit *contact(const QString &jid, bool create = false);
//	void setOffline();
//protected:
//	void loadSettings();
//	void fillContact(JContact *contact, const RosterItem &item);
//	void handleItemAdded(const JID &jid);
//	void handleItemSubscribed(const JID &jid);
//	void handleItemRemoved(const JID &jid);
//	void handleItemUpdated(const JID &jid);
//	void handleItemUnsubscribed(const JID &jid);
//	void handleRoster(const Roster &roster);
//	void handleRosterPresence(const RosterItem &item,
//							  const std::string &resource, Presence::PresenceType presence,
//							  const std::string &msg);
//	void handlePresence(const Presence &presence);
//	void handleSelfPresence(const RosterItem &item, const std::string &resource,
//							Presence::PresenceType presence, const std::string &msg);
//	bool handleSubscriptionRequest(const JID &jid, const std::string &msg);
//	bool handleUnsubscriptionRequest(const JID &jid, const std::string &msg);
//	void handleNonrosterPresence(const Presence &presence);
//	void handleRosterError(const IQ &iq);
//	void handleSubscription(const Subscription &subscription);
//	void handleMetaContact(const MetaContactList &mcList);
//	bool eventFilter(QObject *obj, QEvent *ev);
//private slots:
//	void sendAuthResponse(bool answer);
//	void onSessionCreated(qutim_sdk_0_3::ChatSession *session);
//private:
//	QScopedPointer<JRosterPrivate> p;
//};

}
#endif // JROSTER_H

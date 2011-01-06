#ifndef JACCOUNT_P_H
#define JACCOUNT_P_H

#include "jaccount.h"
#include "muc/jbookmarkmanager.h"
//jreen
#include <jreen/client.h>
//#include <jreen/pubsubmanager.h>
#include "vcard/jvcardmanager.h"
#include <sdk/jabber.h>
namespace jreen
{
class Client;
}

namespace Jabber {

typedef QHash<QString, QHash<QString, QString> > Identities;

class JAccountPrivate : public QObject
{
	Q_OBJECT
	Q_DECLARE_PUBLIC(JAccount)
public:
	inline JAccountPrivate(JAccount *q) : q_ptr(q),keepStatus(false) {}
	inline ~JAccountPrivate() {}
	//jreen
	jreen::Client client;
	JSoftwareDetection *softwareDetection;
	JRoster *roster;
	JMessageSessionManager *messageSessionManager;
	JVCardManager *vCardManager;
	jreen::PubSub::Manager *pubSubManager;
	jreen::PrivateXml *privateXml;
	QPointer<JServiceDiscovery> discoManager;
	JAccount *q_ptr;
	QString passwd;
	QString nick;
	bool keepStatus;
	jreen::Presence::Type status;
	QList<JabberExtension *> extensions;
	JabberParams params;
	QPointer<JMUCManager> conferenceManager;
public slots:	
	void setPresence(jreen::Presence);
	void onConnected();
	void onDisconnected();
	//temporary
	//void handleIQ(const jreen::IQ &iq);
	void initExtensions(const QSet<QString> &features);
public:	//old code
	JConnection *connection;
//	JRoster *roster;
	JConnectionListener *connectionListener;
	JMessageHandler *messageHandler;
//	Presence::PresenceType status;
//	QSet<QString> features;
	Identities identities;
};

}


#endif // JACCOUNT_P_H

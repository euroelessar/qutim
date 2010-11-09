#ifndef JACCOUNT_P_H
#define JACCOUNT_P_H

#include "jaccount.h"
#include "muc/jbookmarkmanager.h"

namespace jreen
{
class Client;
}

namespace Jabber {

typedef QHash<QString, QHash<QString, QString> > Identities;

struct JAccountPrivate
{
	inline JAccountPrivate() : keepStatus(false), autoConnect(false) {}
	inline ~JAccountPrivate() {}
	JConnection *connection;
	JRoster *roster;
	JConnectionListener *connectionListener;
	JMessageHandler *messageHandler;
	QVariantList toVariant(const QList<JBookmark> &list);
	QString passwd;
	QString nick;
	bool keepStatus;
	bool autoConnect;
	Presence::PresenceType status;
	JMUCManager *conferenceManager;
	QPointer<JServiceDiscovery> discoManager;
	QSet<QString> features;
	Identities identities;
	//jreen
	QPointer<jreen::Client> client;
};

}


#endif // JACCOUNT_P_H

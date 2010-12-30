#ifndef JCONNECTION_H
#define JCONNECTION_H

#include <gloox/client.h>
#include <gloox/presencehandler.h>
#include <gloox/disco.h>
#include <gloox/adhoc.h>
#include <gloox/simanager.h>

#include "jconnectiontcpbase.h"

namespace Jabber
{

class JAccount;
class JVCardManager;
class JSoftwareDetection;
class JabberParams;
struct JConnectionPrivate;

class JConnection : public QObject, gloox::PresenceHandler, gloox::LogHandler
{
	Q_OBJECT
public:
	JConnection(JAccount *account);
	~JConnection();
			void setProxy(const QNetworkProxy &proxy);
	void loadSettings();
	void setConnectionPresence(gloox::Presence::PresenceType presence);
	gloox::Client *client();
	gloox::Adhoc *adhoc();
	JVCardManager *vCardManager();
	gloox::SIManager *siManager();
	JConnectionBase *connection();
	JSoftwareDetection *softwareDetection();
	JabberParams params() const;
	void initExtensions();
	void setAvatar(const QString &hex);
	virtual void handleLog(gloox::LogLevel level, gloox::LogArea area, const std::string &message);
protected:
	void handlePresence(const gloox::Presence &presence);
private:
	QScopedPointer<JConnectionPrivate> p;
};
}

#endif // JCONNECTION_H

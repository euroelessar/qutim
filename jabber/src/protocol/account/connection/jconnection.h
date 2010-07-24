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
	using namespace gloox;

	class JAccount;
	class JVCardManager;
	class JSoftwareDetection;
	class JabberParams;
	struct JConnectionPrivate;

	class JConnection : public QObject, PresenceHandler, LogHandler
	{
		Q_OBJECT
		public:
			JConnection(JAccount *account);
			~JConnection();
			void loadSettings();
			void setConnectionPresence(Presence::PresenceType presence);
			Client *client();
			Adhoc *adhoc();
			JVCardManager *vCardManager();
			SIManager *siManager();
			JConnectionBase *connection();
			JSoftwareDetection *softwareDetection();
			JabberParams params() const;
			void initExtensions();
			void setAvatar(const QString &hex);
			virtual void handleLog(LogLevel level, LogArea area, const std::string &message);
		protected:
			void handlePresence(const Presence &presence);
		private:
			QScopedPointer<JConnectionPrivate> p;
	};
}

#endif // JCONNECTION_H

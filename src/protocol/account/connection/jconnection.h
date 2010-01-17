#ifndef JCONNECTION_H
#define JCONNECTION_H

#include <gloox/client.h>
#include <gloox/presencehandler.h>
#include <gloox/disco.h>
#include <gloox/adhoc.h>

#include "jconnectiontcpbase.h"

namespace Jabber
{
	using namespace gloox;

	class JAccount;
	struct JConnectionPrivate;

	class JConnection : public QObject, PresenceHandler
	{
		Q_OBJECT
		public:
			JConnection(JAccount *account);
			~JConnection();
			void loadSettings();
			void setConnectionPresence(Presence::PresenceType presence);
			Client *client();
			Adhoc *adhoc();
			void initExtensions();
		protected:
			void handlePresence(const Presence &presence);
		private:
			QScopedPointer<JConnectionPrivate> p;
	};
}

#endif // JCONNECTION_H

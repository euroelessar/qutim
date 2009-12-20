#ifndef JCONNECTION_H
#define JCONNECTION_H

#include <gloox/client.h>
#include <gloox/presencehandler.h>
#include <gloox/disco.h>
#include <QtNetwork/QNetworkProxy>
#include "jconnectionbase.h"
#include "../jprotocol.h"

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
			void loadSettings();
			void setConnectionPresence(Presence::PresenceType presence);
			Client *client();
		protected:
			void handlePresence(const Presence &presence);
		private:
			JConnectionPrivate *p;
	};
}

#endif // JCONNECTION_H

#ifndef JACCOUNT_H
#define JACCOUNT_H

#include <qutim/account.h>
#include <gloox/presence.h>
#include "jconnection.h"
#include "jinputpassword.h"

namespace Jabber {

	using namespace qutim_sdk_0_3;
	using namespace gloox;

	struct JAccountPrivate;
	//class Roster;
	//class OscarConnection;

	class JAccount : public Account
	{
		Q_OBJECT
		public:
			JAccount(const QString &jid);
			virtual ~JAccount();
			//virtual void setStatus(Status status);
			//Roster *roster();
			//OscarConnection *connection();
			ChatUnit *getUnit(const QString &unitId, bool create = false);
			//void setStatus(const QString &status);
			//void changeStatus(const QString &status);
			const QString &jid();
			const QString &password(bool *ok = 0);
			void setConnectionPresence(Presence::PresenceType presence);
			void autoconnect();
		private:
			void loadSettings();
			//friend class Roster;
			QScopedPointer<JAccountPrivate> p;
	};
} // Jabber namespace

#endif // JACCOUNT_H

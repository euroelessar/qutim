#ifndef JACCOUNT_H
#define JACCOUNT_H

#include <qutim/account.h>

namespace Jabber {

using namespace qutim_sdk_0_3;

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
	private:
		void loadSettings();
		//friend class Roster;
		QScopedPointer<JAccountPrivate> p;
};

} // Jabber namespace

#endif // JACCOUNT_H

#include "../jprotocol.h"
#include "jaccount.h"
#include <QDebug>

//#include "oscarconnection.h"
//#include "roster.h"

namespace Jabber {

	struct JAccountPrivate
	{
		inline JAccountPrivate() : keepStatus(false), autoConnect(false) {}
		inline ~JAccountPrivate() {}
		JConnection *connection;
		QString jid;
		QString passwd;
		bool keepStatus;
		bool autoConnect;
		Presence::PresenceType status;
		//Roster *roster;
	};

	JAccount::JAccount(const QString &jid) : Account(jid, JProtocol::instance()), p(new JAccountPrivate)
	{
		p->jid = jid;
		p->connection = new JConnection(this);
		connect(p->connection, SIGNAL(setStatus(Presence::PresenceType)),
				this, SLOT(endChangeStatus(Presence::PresenceType)));
		loadSettings();
		p->keepStatus = false;
		autoconnect();
		//p->roster = new JRoster(p->connection);
		//p->connection->registerHandler(p->roster);
	}

	JAccount::~JAccount()
	{
	}

	ChatUnit *JAccount::getUnit(const QString &unitId, bool create)
	{
		/*IcqContact *contact = p->roster->contact(unitId);
		if(create && !contact)
		{
			contact = p->roster->sendAddContactRequest(unitId, unitId, not_in_list_group);
		}
		return contact;*/
		return 0;
	}

	/*Roster *IcqAccount::roster()
	{
		return p->roster;
	}*/

	/*OscarConnection *IcqAccount::connection()
	{
		return p->conn;
	}*/

	void JAccount::beginChangeStatus(Presence::PresenceType presence)
	{
		p->connection->setConnectionPresence(presence);
	}

	void JAccount::endChangeStatus(Presence::PresenceType presence)
	{
		Account::setStatus(JProtocol::presenceToStatus(presence));
	}

	void JAccount::autoconnect()
	{
		if (p->autoConnect) {
			if (p->keepStatus)
				beginChangeStatus(p->status);
			else
				beginChangeStatus(Presence::Available);
		}
	}

	void JAccount::loadSettings()
	{
		p->passwd = config().group("general").value("passwd", QString(), Config::Crypted);
		p->autoConnect = config().group("general").value("autoconnect", true);
		p->keepStatus = config().group("general").value("keepstatus", true);
		p->status = static_cast<Presence::PresenceType>(
				config().group("general").value("prevstatus", 8));
	}

	const QString &JAccount::jid()
	{
		return p->jid;
	}

	const QString &JAccount::password(bool *ok)
	{
		if (ok)
			*ok = true;
		if (p->passwd.isEmpty()) {
			JInputPassword *inputPasswd = new JInputPassword(jid());
			if (inputPasswd->exec()) {
				p->passwd = inputPasswd->passwd();
				if (inputPasswd->isSave()) {
					config().group("general").setValue("passwd",
							p->passwd,
							Config::Crypted);
					config().sync();
				}
			} else {
				if (ok)
					*ok = false;
			}
			inputPasswd->close();
			delete inputPasswd;
		}
		return p->passwd;
	}

} // Jabber namespace


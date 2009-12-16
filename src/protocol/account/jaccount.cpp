#include "../jprotocol.h"
#include "jaccount.h"

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
		QString status;
		//Roster *roster;
	};

	JAccount::JAccount(const QString &jid) : Account(jid, JProtocol::instance()), p(new JAccountPrivate)
	{
		p->jid = jid;
		p->connection = new JConnection(this);
		loadSettings();
		p->keepStatus = false;
		if (p->autoConnect)
			autoconnect();
		///setStatus(p.status);
		//p->roster = new JRoster(p->connection);
		//p->connection->registerHandler(p->roster);
		///p->connection->registerHandler(p->roster = new JRoster(this));
		///setStatus(Evil);
	}

	JAccount::~JAccount()
	{
	}

	ChatUnit *JAccount::getUnit(const QString &unitId, bool create)
	{
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

	//void JAccount::setStatus()
	//{

	//}

	/*void IcqAccount::setStatus(Status status)
	{
		Status current = this->status();
		if(status < Offline || status > OnThePhone || current == status)
			return;
		if(current >= Connecting)
		{
			if(status == Offline)
				p->conn->disconnectFromHost(true);
			else
				p->conn->setStatus(status);
		}
		else if(status == Offline)
		{
			p->conn->disconnectFromHost(false);
			foreach(IcqContact *contact, p->roster->contacts())
				contact->setStatus(Offline);
		}
		else if(current == Offline)
		{
			p->conn->setStatus(status);
			p->conn->connectToLoginServer();
		}
		Account::setStatus(status);
	}*/

	/*ChatUnit *IcqAccount::getUnit(const QString &unitId, bool create)
	{
		IcqContact *contact = p->roster->contact(unitId);
		if(create && !contact)
		{
			contact = p->roster->sendAddContactRequest(unitId, unitId, not_in_list_group);
		}
		return contact;
	}*/

	void JAccount::autoconnect()
	{
		if (p->keepStatus) {
			Presence::PresenceType presence = Presence::Invalid;
			if (p->status == "online")
				presence = Presence::Available;
			else if (p->status == "away")
				presence = Presence::Away;
			else if (p->status == "chat")
				presence = Presence::Chat;
			else if (p->status == "dnd")
				presence = Presence::DND;
			else if (p->status == "na")
				presence = Presence::XA;
			else
				presence = Presence::Unavailable;
			if (presence != Presence::Unavailable && presence != Presence::Invalid)
				setConnectionPresence(presence);
		} else {
			setConnectionPresence(Presence::Available);
		}
	}

	void JAccount::setConnectionPresence(Presence::PresenceType presence)
	{
		p->connection->setConnectionPresence(presence);
	}

	void JAccount::loadSettings()
	{
		p->passwd = config().group("general").value("passwd", QString(), Config::Crypted);
		p->autoConnect = config().group("general").value("autoconnect", true);
		p->keepStatus = config().group("general").value("keepstatus", true);
		p->status = config().group("general").value("prevstatus", QString("offline"));
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


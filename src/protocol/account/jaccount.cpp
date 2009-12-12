#include "../jprotocol.h"
#include "jaccount.h"

//#include "oscarconnection.h"
//#include "roster.h"

namespace Jabber {

struct JAccountPrivate
{
	inline JAccountPrivate() : keepStatus(false), autoConnect(false) {}
	inline ~JAccountPrivate() {}
	//JConnection *connection;
	bool keepStatus;
	bool autoConnect;
	QString status;
	//Roster *roster;
};

JAccount::JAccount(const QString &jid) : Account(jid, JProtocol::instance()), p(new JAccountPrivate)
{
	//p->connection = new JConnection(this);
	loadSettings();
	//setStatus(p.status);
	//p->conn->registerHandler(p->roster = new Roster(this));
	//setStatus(Evil);
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

void JAccount::loadSettings()
{
	p->autoConnect = config().group("general").value("autoconnect", false);
	p->keepStatus = config().group("general").value("keepstatus", false);
	p->status = config().group("general").value("prevstatus", QString("offline"));
}

} // Jabber namespace


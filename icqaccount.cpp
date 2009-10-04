#include "icqaccount.h"
#include "icqprotocol.h"
#include "oscarconnection.h"
#include "roster.h"

struct IcqAccountPrivate
{
	OscarConnection *conn;
	Roster *roster;
};

IcqAccount::IcqAccount(const QString &uin) : Account(uin, IcqProtocol::instance()), p(new IcqAccountPrivate)
{
	p->conn = new OscarConnection(this);
	p->conn->registerHandler(p->roster = new Roster(this));
	setStatus(Evil);
}

IcqAccount::~IcqAccount()
{
}

Roster *IcqAccount::roster()
{
	return p->roster;
}

void IcqAccount::setStatus(Status status)
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
		p->conn->disconnectFromHost(false);
	else if(current == Offline)
	{
		p->conn->setStatus(status);
		p->conn->connectToLoginServer();
	}
	Account::setStatus(status);
}

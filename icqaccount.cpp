#include "icqaccount.h"
#include "icqprotocol.h"
#include "oscarconnection.h"

struct IcqAccountPrivate
{
	OscarConnection *conn;
};

IcqAccount::IcqAccount(const QString &uin) : Account(uin, IcqProtocol::instance()), p(new IcqAccountPrivate)
{
	p->conn = new OscarConnection(this);
	setStatus(Evil);
}

IcqAccount::~IcqAccount()
{
}

void IcqAccount::setStatus(Status status)
{
	Status current = this->status();
	if(status < Offline || status > OnThePhone || current == status)
		return;
	if(current >= Connecting)
	{
		if(status == Offline)
		{
		}
		else
		{
			p->conn->setStatus(status);
		}
	}
	else if(status == Offline)
	{
	}
	else if(current == Offline)
	{
		p->conn->setStatus(status);
		p->conn->connectToLoginServer();
	}
	Account::setStatus(status);
}

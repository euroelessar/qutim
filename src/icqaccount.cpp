/****************************************************************************
 *  icqaccount.cpp
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include "icqaccount.h"
#include "icqprotocol.h"
#include "oscarconnection.h"
#include "roster.h"

namespace Icq {

struct IcqAccountPrivate
{
	OscarConnection *conn;
	Roster *roster;
};

IcqAccount::IcqAccount(const QString &uin) : Account(uin, IcqProtocol::instance()), p(new IcqAccountPrivate)
{
	p->conn = new OscarConnection(this);
	p->conn->registerHandler(p->roster = new Roster(this));
}

IcqAccount::~IcqAccount()
{
}

Roster *IcqAccount::roster()
{
	return p->roster;
}

OscarConnection *IcqAccount::connection()
{
	return p->conn;
}

void IcqAccount::setStatus(Status status)
{
	Status current = this->status();
	qDebug("Changing status from %s to %s",
		   qPrintable(statusToString(current, false)),
		   qPrintable(statusToString(status, false)));
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
}

ChatUnit *IcqAccount::getUnit(const QString &unitId, bool create)
{
	IcqContact *contact = p->roster->contact(unitId);
	if(create && !contact)
	{
		contact = p->roster->sendAddContactRequest(unitId, unitId, not_in_list_group);
	}
	return contact;
}

} // namespace Icq

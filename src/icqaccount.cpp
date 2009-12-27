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

namespace Icq
{

struct IcqAccountPrivate
{
	OscarConnection *conn;
	Roster *roster;
	QString name;
	bool avatars;
};

IcqAccount::IcqAccount(const QString &uin) :
	Account(uin, IcqProtocol::instance()), p(new IcqAccountPrivate)
{
	p->conn = new OscarConnection(this);
	p->conn->registerHandler(p->roster = new Roster(this));
	p->avatars = protocol()->config("general").value("avatars", QVariant(true)).toBool();
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
	debug() << QString("Changing status from %1 to %2") .arg(statusToString(current, false)) .arg(statusToString(status, false));
	if (status < Offline || status > OnThePhone || current == status)
		return;
	if (status == Offline) {
		p->conn->disconnectFromHost(false);
		foreach(IcqContact *contact, p->roster->contacts())
			contact->setStatus(Offline);
	} else if (current == Offline) {
		p->conn->setStatus(status);
		status = Connecting;
		p->conn->connectToLoginServer();
	} else
		p->conn->setStatus(status);
	Account::setStatus(status);
}

QString IcqAccount::name() const
{
	if (!p->name.isEmpty())
		return p->name;
	else
		return id();
}

void IcqAccount::setName(const QString &name)
{
	p->name = name;
}

ChatUnit *IcqAccount::getUnit(const QString &unitId, bool create)
{
	IcqContact *contact = p->roster->contact(unitId);
	if (create && !contact) {
		contact = p->roster->sendAddContactRequest(unitId, unitId, not_in_list_group);
	}
	return contact;
}

void IcqAccount::setAvatarsSupport(bool avatars)
{
	p->avatars = avatars;
}

bool IcqAccount::avatarsSupport()
{
	return p->avatars;
}

} // namespace Icq

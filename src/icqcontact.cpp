/****************************************************************************
 *  icqcontact.cpp
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

#include "icqcontact_p.h"
#include "icqaccount.h"
#include "roster.h"

IcqContact::IcqContact(const QString &uin, IcqAccount *account) : Contact(account), p(new IcqContactPrivate)
{
	p->account = account;
	p->uin = uin;
	p->status = Offline;
}

QSet<QString> IcqContact::tags() const
{
	QSet<QString> group;
	QString group_name = p->group_id == 0xffff ? QString() : p->account->roster()->groupId2Name(p->group_id);
	if(!group_name.isNull())
		group.insert(group_name);
	return group;
}

QString IcqContact::id() const
{
	return p->uin;
}

QString IcqContact::name() const
{
	return p->name;
}

Status IcqContact::status() const
{
	return p->status;
}

bool IcqContact::isInList() const
{
	return p->group_id == 0xffff;
}

// TODO: Impl this fucked things)

void IcqContact::sendMessage(const Message &message)
{
}

void IcqContact::setName(const QString &name)
{
}

void IcqContact::setTags(const QSet<QString> &tags)
{
}

void IcqContact::setInList(bool inList)
{
}

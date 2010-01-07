/****************************************************************************
 *  cookie.h
 *
 *  Copyright (c) 2010 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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


#include "cookie.h"
#include "icqcontact.h"
#include "icqaccount.h"
#include "icq_global.h"
#include <QDateTime>

namespace Icq
{

struct CookiePrivate
{
	CookiePrivate(quint64 _id = 0):
		id(_id)
	{
	}

	quint64 id;
	IcqContact *contact;
	IcqAccount *account;
};

Cookie::Cookie(bool generate):
	d_ptr(new CookiePrivate)
{
	Q_D(Cookie);
	if (generate)
		d->id = generateId();
	else
		d->id = 0;
}

Cookie::Cookie(quint64 id):
	d_ptr(new CookiePrivate(id))
{
	Q_D(Cookie);
	d->contact = NULL;
	d->account = NULL;
}

Cookie::Cookie(IcqContact *contact, quint64 id, bool l):
	d_ptr(new CookiePrivate(id))
{
	Q_D(Cookie);
	setContact(contact);
	if (l)
		lock();
}

Cookie::Cookie(IcqAccount *account, quint64 id, bool l):
	d_ptr(new CookiePrivate(id))
{
	Q_D(Cookie);
	d->contact = NULL;
	d->account = account;
	if (l)
		lock();
}

Cookie::Cookie(const Cookie &cookie):
	d_ptr(cookie.d_ptr)
{

}

Cookie &Cookie::operator=(const Cookie &cookie)
{
	d_ptr = cookie.d_ptr;
	return *this;
}

Cookie::~Cookie()
{

}

void Cookie::lock() const
{
	Q_D(const Cookie);
	Q_ASSERT(d->account);
	d->account->cookies().insert(d->id, *this);
}

bool Cookie::unlock() const
{
	Q_D(const Cookie);
	Q_ASSERT(d->account);
	return !d->account->cookies().take(d->id).isEmpty();
}

bool Cookie::isLocked() const
{
	Q_D(const Cookie);
	Q_ASSERT(d->account);
	return d->account->cookies().contains(d->id);
}

bool Cookie::isEmpty() const
{
	return d_func()->id == 0;
}

quint64 Cookie::id() const
{
	return d_func()->id;
}

IcqContact *Cookie::contact() const
{
	return d_func()->contact;
}

void Cookie::setContact(IcqContact *contact)
{
	Q_D(Cookie);
	d->contact = contact;
	d->account = contact->account();
}

IcqAccount *Cookie::account() const
{
	return d_func()->account;
}

void Cookie::setAccount(IcqAccount *account)
{
	d_func()->account = account;
}

quint64 Cookie::generateId()
{
	return QDateTime::currentDateTime().toTime_t();
}

} // namespace Icq

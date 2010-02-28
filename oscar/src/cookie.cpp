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
#include <QTimer>

namespace qutim_sdk_0_3 {

namespace oscar {

class CookiePrivate: public QSharedData
{
public:
	CookiePrivate(quint64 _id = 0):
		id(_id)
	{
	}

	quint64 id;
	IcqContact *contact;
	IcqAccount *account;
	mutable QTimer timer;
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

Cookie::Cookie(IcqContact *contact, quint64 id):
	d_ptr(new CookiePrivate(id))
{
	setContact(contact);
}

Cookie::Cookie(IcqAccount *account, quint64 id):
	d_ptr(new CookiePrivate(id))
{
	Q_D(Cookie);
	d->contact = NULL;
	d->account = account;
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

void Cookie::lock(QObject *receiver, const char *member, int msec) const
{
	Q_D(const Cookie);
	Q_ASSERT(d->account);
	Cookie *cookie = new Cookie(*this);
	d->account->cookies().insert(d->id, cookie);
	if (receiver)
		QObject::connect(cookie, SIGNAL(timeout()), receiver, member);
	d->timer.singleShot(msec, cookie, SLOT(onTimeout()));
}

bool Cookie::unlock() const
{
	Q_D(const Cookie);
	Q_ASSERT(d->account);
	Cookie *cookie = d->account->cookies().take(d->id);
	if (cookie) {
		cookie->d_func()->timer.stop();
		cookie->deleteLater();
		return true;
	} else {
		return false;
	}
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

void Cookie::onTimeout()
{
	emit timeout();
	unlock();
}

} } // namespace qutim_sdk_0_3::oscar

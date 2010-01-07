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

#ifndef COOKIE_H
#define COOKIE_H

#include <QObject>
#include <QSharedPointer>

namespace Icq
{

class IcqContact;
class IcqAccount;
struct CookiePrivate;

class Cookie: public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(Cookie)
	Q_PROPERTY(QString id READ id)
	Q_PROPERTY(IcqContact *contact READ contact WRITE setContact)
	Q_PROPERTY(IcqAccount *account READ account WRITE setAccount)
public:
	Cookie(bool generate = false);
	Cookie(quint64 id);
	Cookie(IcqContact *contact, quint64 id = 0, bool lock = false);
	Cookie(IcqAccount *account, quint64 id = 0, bool lock = false);
	Cookie(const Cookie &cookie);
	Cookie &operator=(const Cookie &cookie);
	virtual ~Cookie();
	void lock() const;
	bool unlock() const;
	bool isLocked() const;
	bool isEmpty() const;
	quint64 id() const;
	IcqContact *contact() const;
	void setContact(IcqContact *contact);
	IcqAccount *account() const;
	void setAccount(IcqAccount *account);
	static quint64 generateId();
private:
	QSharedPointer<CookiePrivate> d_ptr;
};

} // namespace Icq

#endif // COOKIE_H

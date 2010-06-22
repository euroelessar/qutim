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
#include "dataunit.h"

namespace qutim_sdk_0_3 {

namespace oscar {

class IcqContact;
class IcqAccount;
class CookiePrivate;

class LIBOSCAR_EXPORT Cookie: public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(Cookie)
	Q_PROPERTY(QString id READ id)
	Q_PROPERTY(IcqContact *contact READ contact WRITE setContact)
	Q_PROPERTY(IcqAccount *account READ account WRITE setAccount)
public:
	Cookie(bool generate = false);
	Cookie(quint64 id);
	Cookie(IcqContact *contact, quint64 id = 0);
	Cookie(IcqAccount *account, quint64 id = 0);
	Cookie(const Cookie &cookie);
	Cookie &operator=(const Cookie &cookie);
	virtual ~Cookie();
	void lock(QObject *receiver = 0, const char *member = 0, int msec = 30000) const;
	bool unlock() const;
	bool isLocked() const;
	bool isEmpty() const;
	quint64 id() const;
	IcqContact *contact() const;
	void setContact(IcqContact *contact);
	IcqAccount *account() const;
	void setAccount(IcqAccount *account);
	static quint64 generateId();
signals:
	void timeout();
private slots:
	void onTimeout();
private:
	QExplicitlySharedDataPointer<CookiePrivate> d_ptr;
};

template<>
struct toDataUnitHelper<Cookie>
{
	static inline QByteArray toByteArray(const Cookie &data)
	{
		return toDataUnitHelper<quint64>::toByteArray(data.id());
	}
};

template<>
struct fromDataUnitHelper<Cookie>
{
	static inline Cookie fromByteArray(const DataUnit &d)
	{
		return Cookie(d.read<quint64>());
	}
};

} } // namespace qutim_sdk_0_3::oscar

#endif // COOKIE_H

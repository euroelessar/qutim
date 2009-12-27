/****************************************************************************
 *  icqaccount.h
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

#ifndef ICQACCOUNT_H
#define ICQACCOUNT_H

#include <qutim/account.h>

namespace Icq
{

using namespace qutim_sdk_0_3;

struct IcqAccountPrivate;
class Roster;
class OscarConnection;

class IcqAccount: public Account
{
	Q_OBJECT
	Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
	Q_PROPERTY(bool avatarsSupport READ avatarsSupport WRITE setAvatarsSupport)
public:
	IcqAccount(const QString &uin);
	virtual ~IcqAccount();
	virtual void setStatus(Status status);
	virtual QString name() const;
	void setName(const QString &name);
	Roster *roster();
	OscarConnection *connection();
	ChatUnit *getUnit(const QString &unitId, bool create);
	void setAvatarsSupport(bool avatars);
	bool avatarsSupport();
private:
	friend class Roster;
	QScopedPointer<IcqAccountPrivate> p;
};

} // namespace Icq

#endif // ICQACCOUNT_H

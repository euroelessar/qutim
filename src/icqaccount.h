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

using namespace qutim_sdk_0_3;

struct IcqAccountPrivate;
class Roster;

class IcqAccount : public Account
{
	Q_OBJECT
public:
	IcqAccount(const QString &uin);
	virtual ~IcqAccount();
	virtual void setStatus(Status status);
	Roster *roster();
	ChatUnit *getUnit(const QString &unitId, bool create);
private:
	QScopedPointer<IcqAccountPrivate> p;
};

#endif // ICQACCOUNT_H

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
#include "cookie.h"
#include "capability.h"

namespace Icq
{

using namespace qutim_sdk_0_3;

struct IcqAccountPrivate;
class Roster;
class Feedbag;
class OscarConnection;

enum Visibility
{
	AllowAllUsers    = 1,
	BlockAllUsers    = 2,
	AllowPermitList  = 3,
	BlockDenyList    = 4,
	AllowContactList = 5
};

class IcqAccount: public Account
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(IcqAccount)
	Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
	Q_PROPERTY(bool avatarsSupport READ avatarsSupport WRITE setAvatarsSupport)
public:
	IcqAccount(const QString &uin);
	virtual ~IcqAccount();
	virtual void setStatus(Status status);
	virtual QString name() const;
	void setName(const QString &name);
	Feedbag *feedbag();
	OscarConnection *connection();
	ChatUnit *getUnit(const QString &unitId, bool create = false);
	IcqContact *getContact(const QString &id, bool create = false);
	const QHash<QString, IcqContact*> &contacts() const;
	void setAvatarsSupport(bool avatars);
	bool avatarsSupport();
	void setCapability(const Capability &capability, const QString &type = QString());
	bool removeCapability(const Capability &capability);
	bool removeCapability(const QString &type);
	bool containsCapability(const Capability &capability);
	bool containsCapability(const QString &type);
	QList<Capability> capabilities();
	void setVisibility(Visibility visibility);
signals:
	void loginFinished();
public slots:
	void onReconnectTimeout();
private:
	QHash<quint64, Cookie*> &cookies();
	friend class Roster;
	friend class SsiHandler;
	friend class Cookie;
	QScopedPointer<IcqAccountPrivate> d_ptr;
};

} // namespace Icq

#endif // ICQACCOUNT_H

/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Rusanov Peter <peter.rusanov@gmail.com>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#ifndef MRIMACCOUNT_H
#define MRIMACCOUNT_H

#include <qutim/account.h>

#include "roster.h"
#include "mrimconnection.h"
#include "mrimdefs.h"

using namespace qutim_sdk_0_3;

struct MrimAccountPrivate;

class MrimAccount : public Account
{
	Q_OBJECT
public:
    MrimAccount(const QString& email);
	~MrimAccount();
	QString name() const;
    ChatUnit *getUnit(const QString &unitId, bool create = false);
    MrimConnection *connection() const;
    MrimRoster *roster() const;
    void setStatus(Status status);
	void setAccountStatus(const Status &status);
	void setUserInfo(const QMap<QString, QString> &info);

private:
	friend class MrimRoster;
    QScopedPointer<MrimAccountPrivate> p;
};

#endif // MRIMACCOUNT_H


/****************************************************************************
 *  mrimaccount.h
 *
 *  Copyright (c) 2009 by Rusanov Peter <peter.rusanov@gmail.com>
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

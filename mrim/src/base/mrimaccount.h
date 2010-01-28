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

using namespace qutim_sdk_0_3;

struct MrimAccountPrivate;

class MrimAccount : public Account
{
public:
    MrimAccount(const QString& email);
    ChatUnit *getUnit(const QString &unitId, bool create = false);
    MrimConnection *connection() const;
    Roster *roster() const;

private:
    QScopedPointer<MrimAccountPrivate> p;
};

#endif // MRIMACCOUNT_H

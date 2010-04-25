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

#include "mrimprotocol.h"
#include "roster.h"

#include "mrimaccount.h"

struct MrimAccountPrivate
{
    MrimAccountPrivate(MrimAccount *parent)
        : conn(new MrimConnection(parent)), roster(new Roster(parent))
    {
    }

    QScopedPointer<MrimConnection> conn;
    QScopedPointer<Roster> roster;
};

MrimAccount::MrimAccount(const QString& email)
        : Account(email,MrimProtocol::instance()), p(new MrimAccountPrivate(this))
{   
    connect(connection(),SIGNAL(loggedOut()),
            roster(),SLOT(handleLoggedOut()),Qt::QueuedConnection);
    p->conn->registerPacketHandler(p->roster.data());
    p->conn->start(); //TODO: temporary autologin, for debugging
}

ChatUnit *MrimAccount::getUnit(const QString &unitId, bool create)
{
    return p->roster->getContact(unitId);
}

MrimConnection *MrimAccount::connection() const
{ return p->conn.data(); }

Roster *MrimAccount::roster() const
{ return p->roster.data(); }

void MrimAccount::setStatus(Status status)
{

}

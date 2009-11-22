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

#include "mrimaccount.h"
#include "mrimprotocol.h"

MrimAccount::MrimAccount(const QString& email)
        : Account(email,MrimProtocol::instance())
{
}

ChatUnit *MrimAccount::getUnit(const QString &unitId, bool create)
{
    //TODO: roster functionality
    return 0;
}

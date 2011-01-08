/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#ifndef ASTRALSESSIONMANAGER_H
#define ASTRALSESSIONMANAGER_H

#include "astralsession.h"

class AstralSessionManager : public QObject
{
	Q_OBJECT
public:
	explicit AstralSessionManager(AstralAccount *account);

signals:

public slots:

};

#endif // ASTRALSESSIONMANAGER_H

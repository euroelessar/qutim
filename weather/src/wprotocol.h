/****************************************************************************
 * wprotocol.h
 *
 *  Copyright (c) 2010 by Belov Nikita <null@deltaz.org>
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

#ifndef WPROTOCOL_H
#define WPROTOCOL_H

#include <qutim/protocol.h>
#include <qutim/icon.h>

#include "waccount.h"

using namespace qutim_sdk_0_3;

class WProtocol : public Protocol
{
	Q_OBJECT
	Q_CLASSINFO( "Protocol", "weather" )

public:
	WProtocol();
	virtual ~WProtocol();

	virtual QList< Account * > accounts() const;
	virtual Account *account( const QString &id ) const;

private:
	virtual void loadAccounts();

	WAccount *m_account;
};

#endif // WPROTOCOL_H
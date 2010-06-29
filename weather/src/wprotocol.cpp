/****************************************************************************
 * wprotocol.cpp
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

#include "wprotocol.h"

WProtocol::WProtocol()
{
	loadAccounts();
}

WProtocol::~WProtocol()
{
	m_account->deleteLater();
}

QList< Account * > WProtocol::accounts() const
{
	return QList< Account * >() << m_account;
}

Account *WProtocol::account( const QString &id ) const
{
	return m_account;
}

void WProtocol::loadAccounts()
{
	m_account = new WAccount( this );
}

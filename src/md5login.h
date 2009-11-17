/****************************************************************************
 *  md5login.h
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

#ifndef MD5LOGIN_H
#define MD5LOGIN_H

#include "snachandler.h"

class Md5Login : public SNACHandler
{
public:
    Md5Login();
	virtual void handleSNAC(OscarConnection *conn, const SNAC &snac);
};

#endif // MD5LOGIN_H

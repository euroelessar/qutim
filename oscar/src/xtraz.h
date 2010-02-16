/****************************************************************************
 *  xtraz.h
 *
 *  Copyright (c) 2010 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#ifndef XTRAZ_H
#define XTRAZ_H

#include "icq_global.h"
#include "messages.h"

namespace Icq
{

class IcqContact;
class XtrazPrivate;

const Capability MSG_XSTRAZ_SCRIPT(0x3b60b3ef, 0xd82a6c45, 0xa4e09c5a, 0x5e67e865);

class LIBOSCAR_EXPORT XtrazRequest: public ServerMessage
{
public:
	XtrazRequest(IcqContact *contact, const QString &query, const QString &notify);
};

class LIBOSCAR_EXPORT XtrazResponse: public ServerResponseMessage
{
public:
	XtrazResponse(IcqContact *contact, const QString &response, const Cookie &cookie);
};

} // namespace Icq

#endif // XTRAZ_H

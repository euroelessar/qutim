/****************************************************************************
 *  xstatus.h
 *
 *  Copyright (c) 2009 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#ifndef XSTATUS_H_
#define XSTATUS_H_

#include <capability.h>

namespace Icq {

class IcqContact;

class XStatuses
{
public:
	static void handelXStatusCapabilities(IcqContact *contact, const Capabilities &caps, qint8 mood);
};

} // namespace Icq

#endif /* XSTATUS_H_ */

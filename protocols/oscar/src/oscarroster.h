/****************************************************************************
 *  roster.h
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *                        Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#ifndef ROSTER_H
#define ROSTER_H

#include "tlv.h"

namespace qutim_sdk_0_3 {

namespace oscar {

class IcqContact;
class IcqAccount;

class LIBOSCAR_EXPORT RosterPlugin
{
public:
	virtual ~RosterPlugin();
	virtual void statusChanged(IcqContact *contact, Status &status, const TLVMap &tlvs) = 0;
};

} } // namespace qutim_sdk_0_3::oscar

Q_DECLARE_INTERFACE(qutim_sdk_0_3::oscar::RosterPlugin, "org.qutim.oscar.RosterPlugin");

#endif // ROSTER_H

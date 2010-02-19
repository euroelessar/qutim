/****************************************************************************
 *  oscarstatus.h
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

#ifndef OSCARSTATUS_H
#define OSCARSTATUS_H

#include "icq_global.h"
#include "capability.h"
#include <qutim/status.h>

namespace Icq
{

using namespace qutim_sdk_0_3;

enum OscarStatusEnum
{
	// Status FLAGS (used to determine status of other users)
	OscarOnline         = 0x0000,
	OscarAway           = 0x0001,
	OscarDND            = 0x0002,
	OscarNA             = 0x0004,
	OscarOccupied       = 0x0010,
	OscarFFC            = 0x0020,
	OscarInvisible      = 0x0100,

	// Qutim Status Flags
	OscarOffline        = 0xf000,

	// QIP Extended Status flags
	OscarEvil           = 0x3000,
	OscarDepress        = 0x4000,
	OscarHome           = 0x5000,
	OscarWork           = 0x6000,
	OscarLunch          = 0x2001
};

class OscarStatusPrivate;

class LIBOSCAR_EXPORT OscarStatus : public Status
{
public:
	OscarStatus(quint16 status = OscarOffline);
	OscarStatus(Type status);
	OscarStatus(const Status &status);
	OscarStatus(const OscarStatus &status);
	OscarStatus &operator=(quint16 status);
	virtual ~OscarStatus();
	void setCapability(const Capability &capability, const QString &type);
};

} // namespace Icq

#endif // OSCARSTATUS_H

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

namespace qutim_sdk_0_3 {

namespace oscar {

enum OscarStatusEnum
{
	// Status FLAGS (used to determine status of other users)
	OscarOnline         = 0x0000,
	OscarAway           = 0x0001,
	OscarDND            = 0x0002,
	OscarNA             = 0x0004,
	OscarOccupied       = 0x0010,
	OscarInvisible      = 0x0100,
	// Qutim Status Flags
	OscarOffline        = 0x8000,
};

class OscarStatusPrivate;

typedef QHash<QString, Capability> CapabilityHash;

struct LIBOSCAR_EXPORT OscarStatusData
{
	OscarStatusData();
	OscarStatusData(int id, Status::Type type, quint16 flag = 0,
					const QString &iconName = QString(),
					const LocalizedString &name = LocalizedString(),
					const CapabilityHash &caps = CapabilityHash());
	int id;
	Status::Type type;
	quint16 flag;
	QString iconName;
	LocalizedString name;
	CapabilityHash caps;
};

class LIBOSCAR_EXPORT OscarStatus : public Status
{
public:
	OscarStatus(quint16 status = OscarOffline);
	OscarStatus(const OscarStatusData &data);
	OscarStatus(Type status);
	OscarStatus(const Status &status);
	OscarStatus(const OscarStatus &status);
	OscarStatus &operator=(quint16 status);
	OscarStatus &operator=(Status::Type status);
	virtual ~OscarStatus();
	void setData(const OscarStatusData &data);
	quint16 flag() const;
	CapabilityHash capabilities() const;
	void setCapability(const Capability &capability, const QString &type);
	static void registerStatus(OscarStatusData statusData);
protected:
	void setStatusFlag(quint16 status);
	void setStatusType(Status::Type status);
	OscarStatusData getStatusData(int id);
};

} } // namespace qutim_sdk_0_3::oscar

#endif // OSCARSTATUS_H

/****************************************************************************
 *  oscarstatus.cpp
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


#include "oscarstatus_p.h"
#include <qutim/icon.h>
#include <qutim/statusactiongenerator.h>
#include "icqaccount.h"

namespace qutim_sdk_0_3 {

namespace oscar {

static void init_status_list(OscarStatusList &list)
{
	list.insert(OscarAway, Status(Status::Away));
	list.insert(OscarInvisible, Status(Status::Invisible));
	list.insert(OscarDND, Status(Status::DND));
	list.insert(OscarNA, Status(Status::NA));
	list.insert(OscarOnline, Status(Status::Online));
	list.insert(OscarOffline, Status(Status::Offline));
	OscarStatusList::iterator itr = list.begin();
	OscarStatusList::iterator endItr = list.end();
	while (itr != endItr) {
		itr->second.initIcon(QLatin1String("icq"));
		itr->second.setSubtype(itr->first);
		++itr;
	}
	{
		Status status(Status::DND);
		status.setSubtype(OscarOccupied);
		status.setIcon(Icon(QLatin1String("user-busy-occupied-icq")));
		status.setName(QT_TRANSLATE_NOOP("Status", "Busy"));
		list.insert(OscarOccupied, status);
	}
	foreach (const OscarStatusPair &status, list)
		MenuController::addAction(new StatusActionGenerator(status.second), &IcqAccount::staticMetaObject);
}
Q_GLOBAL_STATIC_WITH_INITIALIZER(OscarStatusList, statusList, init_status_list(*x));

void OscarStatusList::insert(quint16 id, const Status &status)
{
	OscarStatusPair data(id, status);
	iterator itr = begin();
	iterator endItr = end();
	while (itr != endItr) {
		if (id > itr->first) {
			QList<OscarStatusPair>::insert(itr, data);
			return;
		}
		++itr;
	}
	push_back(data);
}

CapsTypes &capsTypes()
{
	static CapsTypes capsTypes;
	return capsTypes;
}

qutim_sdk_0_3::Status oscarStatusToQutim(quint16 status)
{
	foreach (const OscarStatusPair &itr, *statusList()) {
		if (status & itr.first)
			return itr.second;
	}
	return statusList()->last().second;
}

OscarStatus::OscarStatus(quint16 status) :
	Status(oscarStatusToQutim(status))
{
	initIcon("icq");
}

OscarStatus::OscarStatus(Status::Type status) :
	Status(status)
{
	initIcon("icq");
	foreach (const OscarStatusPair &itr, *statusList()) {
		if (itr.second.type() == status) {
			*reinterpret_cast<Status*>(this) = itr.second;
			break;
		}
	}
}

OscarStatus::OscarStatus(const Status &status):
	Status(status)
{
	setSubtype(status.subtype());
	setStatusType(status.type());
}

OscarStatus::OscarStatus(const OscarStatus &status):
	Status(status)
{
}

OscarStatus::~OscarStatus()
{
}

OscarStatus &OscarStatus::operator=(quint16 status)
{
	*this = OscarStatus(oscarStatusToQutim(status));
	return *this;
}

OscarStatus &OscarStatus::operator=(Status::Type status)
{
	setStatusType(status);
	return *this;
}

void OscarStatus::setCapability(const Capability &capability, const QString &type)
{
	capsTypes().insert(type);
	CapsList caps = capabilities();
	caps.insert(type, capability);
	setProperty("capabilities", QVariant::fromValue(caps));
}

QHash<QString, Capability> OscarStatus::capabilities()
{
	return property<CapsList>("capabilities", CapsList());
}

void OscarStatus::registerStatus(quint16 statusId, OscarStatus oscarStatus)
{
	statusList()->insert(statusId, oscarStatus);
}

void OscarStatus::setStatusType(Status::Type status)
{
	if (subtype() == 0 && status != Online && status != Connecting) {
		foreach (const OscarStatusPair &itr, *statusList()) {
			if (itr.second.type() == status) {
				*reinterpret_cast<Status*>(this) = itr.second;
				return;
			}
		}
	}
	setType(status);
	initIcon("icq");
}

} } // namespace qutim_sdk_0_3::oscar

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
	//list.insert(OscarFFC, Status(Status::FreeChat));
	list.insert(OscarOnline, Status(Status::Online));
	list.insert(OscarOffline, Status(Status::Offline));
	OscarStatusList::iterator itr = list.begin();
	OscarStatusList::iterator endItr = list.end();
	while (itr != endItr) {
		itr->initIcon(QLatin1String("icq"));
		itr->setSubtype(itr.key());
		++itr;
	}
	{
		Status status(Status::DND);
		status.setSubtype(OscarOccupied);
		status.setIcon(Icon(QLatin1String("user-busy-occupied-icq")));
		status.setName(QT_TRANSLATE_NOOP("Status", "Busy"));
		list.insert(OscarOccupied, status);
	}
	foreach (const Status &status, list)
		MenuController::addAction(new StatusActionGenerator(status), &IcqAccount::staticMetaObject);
}
Q_GLOBAL_STATIC_WITH_INITIALIZER(OscarStatusList, statusList, init_status_list(*x));

CapsTypes &capsTypes()
{
	static CapsTypes capsTypes;
	return capsTypes;
}

qutim_sdk_0_3::Status oscarStatusToQutim(quint16 status)
{
	const OscarStatusList &list = *statusList();
	if (status == OscarOffline)
		return list.value(OscarOffline);
	Status online = list.value(OscarOnline);
	if (status & OscarInvisible)
		return list.value(OscarInvisible, online);
	else /* if (status & OscarEvil)
		return list.value(OscarEvil, online);
	else if(status & OscarDepress)
		return list.value(OscarDepress, online);
	else if(status & OscarHome)
		return list.value(OscarHome, online);
	else if(status & OscarWork)
		return list.value(OscarWork, online);
	else if(status & OscarLunch)
		return list.value(OscarLunch, online);
	else */ if (status & OscarDND)
		return list.value(OscarDND, online);
	else if (status & OscarOccupied)
		return list.value(OscarOccupied, online);
	else if (status & OscarNA)
		return list.value(OscarNA, online);
	else if (status & OscarAway)
		return list.value(OscarAway, online);
	else if (status & OscarFFC)
		return list.value(OscarFFC, online);
	else
		return online;
}

quint16 qutimStatusToOscar(const Status &status)
{
	switch (status.type()) {
	case Status::Away:
		return 0x0001;
	case Status::DND:
		return 0x0013;
	case Status::NA:
		return 0x0005;
//	case Status::FreeChat:
//		return 0x0020;
//	case Evil:
//		return 0x3000;
//	case Depression:
//		return 0x4000;
	case Status::Invisible:
		return 0x0100;
//	case AtHome:
//		return 0x5000;
//	case AtWork:
//		return 0x6000;
//	case OutToLunch:
//		return 0x2001;
	default:
		return 0x0000;
	}
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
	setSubtype(qutimStatusToOscar(status));
}

OscarStatus::OscarStatus(const Status &status):
	Status(status)
{
	initIcon("icq");
	if (status.subtype() == 0 && status.type() != Online)
		setSubtype(qutimStatusToOscar(status));
}

OscarStatus::OscarStatus(const OscarStatus &status):
	Status(status)
{
	initIcon("icq");
}

OscarStatus::~OscarStatus()
{
}

OscarStatus &OscarStatus::operator=(quint16 status)
{
	*this = OscarStatus(oscarStatusToQutim(status));
	return *this;
}

void OscarStatus::setCapability(const Capability &capability, const QString &type)
{
	capsTypes().insert(type);
	CapsList caps = property<CapsList>("capabilities", CapsList());
	caps.insert(type, capability);
	setProperty("capabilities", QVariant::fromValue(caps));
}

} } // namespace qutim_sdk_0_3::oscar

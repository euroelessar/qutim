/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/


#include "oscarstatus_p.h"
#include <qutim/icon.h>
#include <qutim/statusactiongenerator.h>
#include "icqaccount.h"

namespace qutim_sdk_0_3 {

namespace oscar {

static OscarStatusList init_status_list()
{
    OscarStatusList list;
	list.insert(OscarStatusData(OscarAway, Status::Away));
	list.insert(OscarStatusData(OscarInvisible, Status::Invisible));
	list.insert(OscarStatusData(OscarDND, Status::DND));
	list.insert(OscarStatusData(OscarNA, Status::NA));
	list.insert(OscarStatusData(OscarOnline, Status::Online));
	list.insert(OscarStatusData(OscarOffline, Status::Offline));
	list.insert(OscarStatusData(OscarOccupied,
								Status::DND,
								OscarOccupied,
								"busy-occupied",
								QT_TRANSLATE_NOOP("Status", "Busy")));
	foreach (const OscarStatusData &data, list)
		MenuController::addAction<IcqAccount>(new StatusActionGenerator(OscarStatus(data)));
    return list;
}
Q_GLOBAL_STATIC_WITH_ARGS(OscarStatusList, statusList, (init_status_list()))

void OscarStatusList::insert(const OscarStatusData &data)
{
	iterator itr = begin();
	iterator endItr = end();
	while (itr != endItr) {
		if (itr->flag > data.flag) {
			QList<OscarStatusData>::insert(itr, data);
			return;
		}
		++itr;
	}
	push_back(data);
}

CapsTypes &statusDataCapsTypes()
{
	static CapsTypes capsTypes;
	return capsTypes;
}

CapsTypes &capsTypes()
{
	static CapsTypes capsTypes;
	return capsTypes;
}

OscarStatusData::OscarStatusData()
{
}

OscarStatusData::OscarStatusData(int _id, Status::Type _type) :
	id (_id), type(_type), flag(_id)
{
}

OscarStatusData::OscarStatusData(int _id, Status::Type _type, quint16 _flag, const QString &_icon,
								 const LocalizedString &_name, const CapabilityHash &_caps):
	id (_id), type(_type), flag(_flag), iconName(_icon), name(_name), caps(_caps)
{
}

OscarStatus::OscarStatus(quint16 status)
{
	setStatusFlag(status);
}

OscarStatus::OscarStatus(const OscarStatusData &data)
{
	setData(data);
}

OscarStatus::OscarStatus(Status::Type status) :
	Status(status)
{
	setStatusType(status);
}

OscarStatus::OscarStatus(const Status &status):
	Status(status)
{
	initIcon("icq");
	if (subtype() == 0) {
		setStatusType(status.type());
	} else {
		foreach (const OscarStatusData &data, *statusList()) {
			if (data.id == subtype()) {
				setData(data);
				break;
			}
		}
	}
}

OscarStatus::OscarStatus(const OscarStatus &status):
	Status(status)
{
}

OscarStatus::~OscarStatus()
{
}

void OscarStatus::setData(const OscarStatusData &data)
{
	setType(data.type);
	initIcon("icq");
	setSubtype(data.id);
	if (!data.iconName.isEmpty())
		setIcon(Icon(QString("user-%1-icq").arg(data.iconName)));
	if (!data.name.original().isEmpty())
		setName(data.name);
	setProperty("statusFlag", data.flag);
	// Capabilities...
	CapabilityHash caps = capabilities();
	CapsTypes types = statusDataCapsTypes();
	CapabilityHash::const_iterator itr = data.caps.constBegin();
	CapabilityHash::const_iterator endItr = data.caps.constEnd();
	while (itr != endItr) {
		types.remove(itr.key());
		caps.insert(itr.key(), itr.value());
		++itr;
	}
	foreach (const QString &type, types)
		caps.remove(type);
	setProperty("capabilities", QVariant::fromValue(caps));
}

quint16 OscarStatus::flag() const
{
	return property<quint16>("statusFlag", 0);
}

CapabilityHash OscarStatus::capabilities() const
{
	return property<CapabilityHash>("capabilities", CapabilityHash());
}

void OscarStatus::setCapability(const QString &type, const Capability &capability)
{
	capsTypes().insert(type);
	CapabilityHash caps = capabilities();
	caps.insert(type, capability);
	setProperty("capabilities", QVariant::fromValue(caps));
}

void OscarStatus::removeCapability(const QString &type)
{
	CapabilityHash caps = capabilities();
	caps.remove(type);
	setProperty("capabilities", QVariant::fromValue(caps));
}

OscarStatus &OscarStatus::operator=(quint16 status)
{
	setStatusFlag(status);
	return *this;
}

OscarStatus &OscarStatus::operator=(Status::Type status)
{
	setStatusType(status);
	return *this;
}

void OscarStatus::registerStatus(OscarStatusData statusData)
{
	CapabilityHash::const_iterator itr = statusData.caps.constBegin();
	CapabilityHash::const_iterator endItr = statusData.caps.constEnd();
	while (itr != endItr) {
		statusDataCapsTypes().insert(itr.key());
		capsTypes().insert(itr.key());
		++itr;
	}
	statusList()->insert(statusData);
}

bool OscarStatus::setStatusFlag(quint16 status)
{
	foreach (const OscarStatusData &data, *statusList()) {
		if ((status == 0 && data.flag == 0) || (data.flag & status)) {
			setData(data);
			return true;
		}
	}
	return false;
}

void OscarStatus::setStatusType(Status::Type status)
{
	if (status == Connecting)
		return;
	foreach (const OscarStatusData &data, *statusList()) {
		if (data.type == status) {
			setData(data);
			break;
		}
	}
}

OscarStatusData OscarStatus::getStatusData(int id)
{
	foreach (const OscarStatusData &data, *statusList()) {
		if (data.id == id)
			return data;
	}
	return OscarStatusData();
}

} } // namespace qutim_sdk_0_3::oscar


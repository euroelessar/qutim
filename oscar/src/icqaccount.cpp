/****************************************************************************
 *  icqaccount.cpp
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

#include "icqaccount.h"
#include "icqprotocol.h"
#include "oscarconnection.h"
#include "roster.h"
#include "buddycaps.h"
#include <qutim/systeminfo.h>
#include <QTimer>

namespace Icq
{

struct IcqAccountPrivate
{
	OscarConnection *conn;
	Roster *roster;
	QString name;
	bool avatars;
	QHash<quint64, Cookie*> cookies;
	Capabilities caps;
	QHash<QString, Capability> typedCaps;
	Status lastStatus;
	QTimer reconnectTimer;
};

IcqAccount::IcqAccount(const QString &uin) :
	Account(uin, IcqProtocol::instance()), d_ptr(new IcqAccountPrivate)
{
	Q_D(IcqAccount);
	d->conn = new OscarConnection(this);
	d->conn->registerHandler(d->roster = new Roster(this));
	d->lastStatus = Offline; // TODO: load the value from the account config.
	d->avatars = protocol()->config("general").value("avatars", QVariant(true)).toBool();

	// ICQ UTF8 Support
	d->caps.append(ICQ_CAPABILITY_UTF8);
	// Buddy Icon
	d->caps.append(ICQ_CAPABILITY_AIMICON);
	// RTF messages
	//d->caps.append(ICQ_CAPABILITY_RTFxMSGS);
	// qutIM some shit
	d->caps.append(Capability(0x69716d75, 0x61746769, 0x656d0000, 0x00000000));
	d->caps.append(Capability(0x09461343, 0x4c7f11d1, 0x82224445, 0x53540000));
	// HTML messages
	d->caps.append(ICQ_CAPABILITY_HTMLMSGS);
	// ICQ typing
	d->caps.append(ICQ_CAPABILITY_TYPING);
	// Xtraz
	d->caps.append(ICQ_CAPABILITY_XTRAZ);
	// Messages on channel 2
	d->caps.append(ICQ_CAPABILITY_SRVxRELAY);
	// Short capability support
	d->caps.append(ICQ_CAPABILITY_SHORTCAPS);

	// qutIM version info
	DataUnit version;
	version.appendData(QByteArray("qutim"));
	version.appendSimple<quint8>(SystemInfo::getSystemTypeID());
	version.appendSimple<quint32>(qutimVersion());
	version.appendSimple<quint8>(0x00);
	version.appendSimple<quint32>(SystemInfo::getSystemVersionID());
	version.appendSimple<quint8>(0x00); // 5 bytes more to 16
	d->caps.append(Capability(version.data()));
}

IcqAccount::~IcqAccount()
{
}

Roster *IcqAccount::roster()
{
	Q_D(IcqAccount);
	return d->roster;
}

OscarConnection *IcqAccount::connection()
{
	Q_D(IcqAccount);
	return d->conn;
}

void IcqAccount::setStatus(Status status)
{
	Q_D(IcqAccount);
	Status current = this->status();
	debug() << QString("Changing status from %1 to %2")
			.arg(statusToString(current, false))
			.arg(statusToString(status, false));
	if (current == status) {
		if (status == Offline) {
			d->lastStatus = status;
			d->reconnectTimer.stop();
		}
		return;
	}
	if (status == Offline) {
		if (d->conn->isConnected()) {
			d->conn->disconnectFromHost(false);
			d->lastStatus = status;
		} else if (d->conn->error() == AbstractConnection::NoError ||
				   d->conn->error() == AbstractConnection::ReservationLinkError ||
				   d->conn->error() == AbstractConnection::ReservationMapError)
		{
			Config config = protocol()->config();
			if (config.group("reconnect").value("enabled", true)) {
				quint32 time = config.group("reconnect").value("time", 3000);
				d->reconnectTimer.singleShot(time, this, SLOT(onReconnectTimeout()));
			}
		}
		foreach(IcqContact *contact, d->roster->contacts())
			contact->setStatus(Offline);
	} else if (status == ConnectingStop && current == Connecting) {
		status = d->lastStatus;
		d->conn->sendStatus(status);
		d->conn->metaInfo()->sendShortInfoRequest(d->conn, this); // Requesting own information.
		emit loginFinished();
	} else if (status >= Online && status <= OnThePhone) {
		d->lastStatus = status;
		if (current == Offline) {
			status = Connecting;
			d->conn->connectToLoginServer();
			d->reconnectTimer.stop();
		} else {
			d->conn->sendStatus(status);
		}
	}
	Account::setStatus(status);
}

QString IcqAccount::name() const
{
	Q_D(const IcqAccount);
	if (!d->name.isEmpty())
		return d->name;
	else
		return id();
}

void IcqAccount::setName(const QString &name)
{
	Q_D(IcqAccount);
	d->name = name;
}

ChatUnit *IcqAccount::getUnit(const QString &unitId, bool create)
{
	return getContact(unitId, create);
}

IcqContact *IcqAccount::getContact(const QString &id, bool create)
{
	Q_D(IcqAccount);
	IcqContact *contact = d->roster->contact(id);
	if (create && !contact)
		contact = d->roster->sendAddContactRequest(id, id, not_in_list_group);
	return contact;
}

void IcqAccount::setAvatarsSupport(bool avatars)
{
	Q_D(IcqAccount);
	d->avatars = avatars;
}

bool IcqAccount::avatarsSupport()
{
	Q_D(IcqAccount);
	return d->avatars;
}

void IcqAccount::setCapability(const Capability &capability, const QString &type)
{
	Q_D(IcqAccount);
	if (type.isEmpty())
		d->caps.push_back(capability);
	else
		d->typedCaps.insert(type, capability);
}

bool IcqAccount::removeCapability(const Capability &capability)
{
	Q_D(IcqAccount);
	return d->caps.removeOne(capability);
}

bool IcqAccount::removeCapability(const QString &type)
{
	Q_D(IcqAccount);
	return d->typedCaps.remove(type) > 0;
}

bool IcqAccount::containsCapability(const Capability &capability)
{
	Q_D(IcqAccount);
	if (d->caps.contains(capability))
		return true;
	foreach (const Capability &cap, d->typedCaps) {
		if (cap == capability)
			return true;
	}
	return false;
}

bool IcqAccount::containsCapability(const QString &type)
{
	Q_D(IcqAccount);
	return d->typedCaps.contains(type);
}

QList<Capability> IcqAccount::capabilities()
{
	Q_D(IcqAccount);
	QList<Capability> caps = d->caps;
	foreach (const Capability &cap, d->typedCaps)
		caps << cap;
	return caps;
}

void IcqAccount::onReconnectTimeout()
{
	Q_D(IcqAccount);
	if (status() == Offline)
		setStatus(d->lastStatus);
}

QHash<quint64, Cookie*> &IcqAccount::cookies()
{
	Q_D(IcqAccount);
	return d->cookies;
}

} // namespace Icq

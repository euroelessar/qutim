/****************************************************************************
 *  icqaccount.cpp
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

#include "icqaccount_p.h"
#include "icqprotocol.h"
#include "icqcontact_p.h"
#include "oscarconnection.h"
#include "roster_p.h"
#include "buddycaps.h"
#include "oscarstatus.h"
#include "buddypicture.h"
#include <qutim/status.h>
#include <qutim/systeminfo.h>
#include <qutim/contactlist.h>
#include <QTimer>
#include <qutim/objectgenerator.h>

namespace qutim_sdk_0_3 {

namespace oscar {

PasswordValidator::PasswordValidator(QObject *parent) :
	QValidator(parent)
{
}

PasswordValidator::State PasswordValidator::validate(QString &input, int &pos) const
{
	if (input.isEmpty())
		return Intermediate;
	if (input.size() > 8)
		return Invalid;
	else
		return Acceptable;
}

IcqAccount::IcqAccount(const QString &uin) :
	Account(uin, IcqProtocol::instance()), d_ptr(new IcqAccountPrivate)
{
	Q_D(IcqAccount);
	d->reconnectTimer.setSingleShot(true);
	connect(&d->reconnectTimer, SIGNAL(timeout()), SLOT(onReconnectTimeout()));
	ConfigGroup cfg = config("general");
	d->conn = new OscarConnection(this);
	d->conn->registerHandler(d->feedbag = new Feedbag(this));
	d->conn->registerHandler(new Roster(this));
	d->conn->registerHandler(new BuddyPicture(this, this));
	d->conn->registerHandler(d->messagesHandler = new MessagesHandler(this, this));
	d->lastStatus = static_cast<Status::Type>(cfg.value<int>("lastStatus", Status::Offline));

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
	version.append(QByteArray("qutim"));
	version.append<quint8>(SystemInfo::getSystemTypeID());
	version.append<quint32>(qutimVersion());
	version.append<quint8>(0x00);
	version.append<quint32>(SystemInfo::getSystemVersionID());
	version.append<quint8>(0x00); // 5 bytes more to 16
	d->caps.append(Capability(version.data()));

	foreach(const ObjectGenerator *gen, moduleGenerators<RosterPlugin>()) {
		RosterPlugin *plugin = gen->generate<RosterPlugin>();
		d->rosterPlugins << plugin;
	}

	if (cfg.value("autoconnect", false))
		setStatus(d->lastStatus);
}

IcqAccount::~IcqAccount()
{
}

Feedbag *IcqAccount::feedbag()
{
	Q_D(IcqAccount);
	return d->feedbag;
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
	// TODO: Check for text field
	if (current.type() == status.type()) {
		if (status.type() == Status::Offline) {
			d->lastStatus = status;
			d->reconnectTimer.stop();
		}
		//return;
	}
	if (status.type() == Status::Offline) {
		if (d->conn->isConnected()) {
			d->conn->disconnectFromHost(false);
			d->lastStatus = status;
		} else if (d->conn->error() == AbstractConnection::NoError ||
				   d->conn->error() == AbstractConnection::ReservationLinkError ||
				   d->conn->error() == AbstractConnection::ReservationMapError)
		{
			ConfigGroup config = protocol()->config().group("reconnect");
			if (config.value("enabled", true)) {
				quint32 time = config.value("time", 3000);
				d->reconnectTimer.start(time);
			}
		} else if (d->conn->error() == AbstractConnection::MismatchNickOrPassword) {
			Account::setStatus(status);
			config().group("general").setValue("passwd", QString(), Config::Crypted);
			setStatus(d->lastStatus);
			return;
		}
		OscarStatus stat;
		foreach(IcqContact *contact, d->contacts)
			contact->setStatus(stat);
	} else {
		d->lastStatus = status;
		if (current == Status::Offline) {
			d->reconnectTimer.stop();
			QString pass = password();
			if (!pass.isEmpty()) {
				status = Status::Connecting;
				d->conn->connectToLoginServer(pass);
			} else {
				status = OscarStatus();
			}
		} else {
			d->conn->sendStatus(status);
		}
	}
	config().group("general").setValue("lastStatus", status.type());
	config().sync();
	Account::setStatus(status);
	emit statusChanged(status);
}

void IcqAccount::setStatus(OscarStatusEnum status)
{
	setStatus(OscarStatus(status));
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
	IcqContact *contact = d->contacts.value(id);
	if (create && !contact) {
		contact = new IcqContact(id, this);
		d->contacts.insert(id, contact);
		emit contactCreated(contact);
		//if (ContactList::instance())
		//	ContactList::instance()->addContact(contact);
	}
	return contact;
}

const QHash<QString, IcqContact*> &IcqAccount::contacts() const
{
	Q_D(const IcqAccount);
	return d->contacts;
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

void IcqAccount::setVisibility(Visibility visibility)
{
	FeedbagItem item = feedbag()->type(SsiVisibility, Feedbag::CreateItem).first();
	TLV data(0x00CA);
	data.append<quint8>(visibility);
	item.setField(data);
	item.setField<qint32>(0x00C9, 0xffffffff);
	item.update();
}

void IcqAccount::registerRosterPlugin(RosterPlugin *plugin)
{
	Q_D(IcqAccount);
	d->rosterPlugins << plugin;
}

void IcqAccount::registerMessagePlugin(MessagePlugin *plugin)
{
	Q_D(IcqAccount);
	d->messagesHandler->registerMessagePlugin(plugin);
}

void IcqAccount::registerTlv2711Plugin(Tlv2711Plugin *plugin)
{
	Q_D(IcqAccount);
	d->messagesHandler->registerTlv2711Plugin(plugin);
}

QHostAddress IcqAccount::localAddress()
{
	return d_func()->conn->socket()->localAddress();
}

void IcqAccount::updateSettings()
{
	Q_D(IcqAccount);
	d->avatars = protocol()->config("general").value("avatars", true);
}

void IcqAccount::onReconnectTimeout()
{
	Q_D(IcqAccount);
	if (status() == Status::Offline)
		setStatus(d->lastStatus);
}

QHash<quint64, Cookie*> &IcqAccount::cookies()
{
	Q_D(IcqAccount);
	return d->cookies;
}

QString IcqAccount::password()
{
	QString password = config().group("general").value("passwd", QString(), Config::Crypted);
	if (password.isEmpty()) {
		PasswordDialog *dialog = PasswordDialog::request(this);
		dialog->setValidator(new PasswordValidator(dialog));
		if (dialog->exec() == PasswordDialog::Accepted) {
			password = dialog->password();
			if (dialog->remember()) {
				config().group("general").setValue("passwd", password, Config::Crypted);
				config().sync();
			}
		}
		delete dialog;
	}
	return password;
}

} } // namespace qutim_sdk_0_3::oscar

/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "icqaccount_p.h"
#include "icqprotocol.h"
#include "icqcontact_p.h"
#include "oscarconnection.h"
#include "oscarroster_p.h"
#include "buddycaps.h"
#include "oscarstatus.h"
#include "inforequest_p.h"
#include "metainfo/updateaccountinfometarequest.h"
#include <qutim/libqutim_version.h>
#include <qutim/status.h>
#include <qutim/systeminfo.h>
#include <qutim/objectgenerator.h>
#include <QTimer>
#include <QMetaMethod>

namespace qutim_sdk_0_3 {

namespace oscar {

PasswordValidator::PasswordValidator(QObject *parent) :
	QValidator(parent)
{
}

PasswordValidator::State PasswordValidator::validate(QString &input, int &pos) const
{
	Q_UNUSED(pos);
	if (input.isEmpty())
		return Intermediate;
	if (input.size() > 8)
		return Invalid;
	else
		return Acceptable;
}

QString IcqAccountPrivate::password()
{
	Q_Q(IcqAccount);
	QString password;
	if (!passwd.isEmpty()) {
		password = passwd;
		passwd.clear();
	} else {
		Config cfg = q->config("general");
		password = cfg.value("passwd", QString(), Config::Crypted);
		if (password.isEmpty()) {
			PasswordDialog *dialog = PasswordDialog::request(q);
			dialog->setValidator(new PasswordValidator(dialog));
			QObject::connect(dialog, SIGNAL(entered(QString,bool)), q, SLOT(onPasswordEntered(QString,bool)));
			QObject::connect(dialog, SIGNAL(rejected()), dialog, SLOT(deleteLater()));
		}
	}
	return password;
}

void IcqAccountPrivate::loadRoster()
{
	Q_Q(IcqAccount);
	QMultiMap<quint16, FeedbagItemHandler*> handlers;
	foreach (const ObjectGenerator *gen, ObjectGenerator::module<FeedbagItemHandler>()) {
		FeedbagItemHandler *handler = gen->generate<FeedbagItemHandler>();
		handlers.insert(handler->priority(), handler);
	}
	QMapIterator<quint16, FeedbagItemHandler*> i(handlers);
	i.toBack();
	while (i.hasPrevious())
		feedbag->registerHandler(i.previous().value());

	conn->registerHandler(buddyPicture = new BuddyPicture(q, q));

	foreach(const ObjectGenerator *gen, ObjectGenerator::module<RosterPlugin>()) {
		RosterPlugin *plugin = gen->generate<RosterPlugin>();
		rosterPlugins << plugin;
	}
}

void IcqAccount::onPasswordEntered(const QString &password, bool remember)
{
	Q_D(IcqAccount);
	PasswordDialog *dialog = qobject_cast<PasswordDialog*>(sender());
	if (!dialog)
		return;
	if (remember) {
		Config cfg = config("general");
		cfg.setValue("passwd", password, Config::Crypted);
	}
	dialog->deleteLater();
	d->passwd = password;
	setStatus(d->lastStatus);
}

IcqAccount::IcqAccount(const QString &uin) :
	Account(uin, IcqProtocol::instance()), d_ptr(new IcqAccountPrivate)
{
	Q_D(IcqAccount);
	setInfoRequestFactory(new IcqInfoRequestFactory(this));
	d->q_ptr = this;
	d->messageSender.reset(new MessageSender(this));
	Config cfg = config("general");
	d->htmlEnabled = cfg.value("htmlEnabled", false);
	d->conn = new OscarConnection(this);
	d->conn->registerHandler(d->feedbag = new Feedbag(this));
	{
		Config statusCfg = cfg.group("lastStatus");
		int type = statusCfg.value("type", static_cast<int>(Status::Offline));
		if (type >= Status::Online && type <= Status::Offline) {
			OscarStatus status(Status::Offline);
			OscarStatus lastStatus;
			lastStatus.setType(static_cast<Status::Type>(type));
			lastStatus.setSubtype(statusCfg.value("subtype", 0));
			statusCfg.beginGroup("capabilities");
			foreach (const QString &type, statusCfg.childKeys()) {
				Capability cap(statusCfg.value("subtype", QString()));
				lastStatus.setCapability(cap, type);
			}
			statusCfg.endGroup();
			d->lastStatus = lastStatus;
			Account::setStatus(status);
		}
	}

	// ICQ UTF8 Support
	d->caps.append(ICQ_CAPABILITY_UTF8);
	// Buddy Icon
	d->caps.append(ICQ_CAPABILITY_AIMICON);
	// RTF messages
	//d->caps.append(ICQ_CAPABILITY_RTFxMSGS);
	// qutIM some shit
	d->caps.append(Capability(0x69716d75, 0x61746769, 0x656d0000, 0x00000000));
	d->caps.append(Capability(0x09461343, 0x4c7f11d1, 0x82224445, 0x53540000));
	if (d->htmlEnabled) {
		// HTML messages
		d->caps.append(ICQ_CAPABILITY_HTMLMSGS);
	}
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
	version.append<quint32>(qutim_sdk_0_3::version());
	version.append<quint8>(0x00);
	version.append<quint32>(SystemInfo::getSystemVersionID());
	version.append<quint8>(0x00); // 5 bytes more to 16
	d->caps.append(Capability(version.data()));

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

AbstractConnection *IcqAccount::connection()
{
	return d_func()->conn;
}

const AbstractConnection *IcqAccount::connection() const
{
	return d_func()->conn;
}

void IcqAccount::finishLogin()
{
	Q_D(IcqAccount);
	d->conn->sendStatus(d->lastStatus);
	Account::setStatus(d->lastStatus);
	emit loginFinished();
}

void IcqAccount::setStatus(Status status_helper)
{
	Q_D(IcqAccount);
	Status status = OscarStatus(status_helper);
	Status current = this->status();
	debug() << Q_FUNC_INFO << current << "->" << status;
    if (current.type() == Status::Connecting
            && status.type() > Status::Offline) {
		d->lastStatus = status;
		if (d->conn->state() == QAbstractSocket::UnconnectedState
		        && status != Status::Connecting) {
			d->conn->connectToLoginServer(QString());
		}
		return;
	}
	if (current.type() == status.type() && status.type() == Status::Offline) {
//		// Disable reconnecting
//		status.setChangeReason(Status::ByUser);
		Account::setStatus(status);
		emit statusChanged(status, current);
		return;
	}
	if (status.type() == Status::Offline) {
		QAbstractSocket::SocketState state = d->conn->state();
		if (state != QTcpSocket::UnconnectedState) {
			d->conn->disconnectFromHost(state != QTcpSocket::ConnectedState);
//			status.setChangeReason(Status::ByUser);
			d->lastStatus = status;
		} else if (d->conn->error() == AbstractConnection::NoError ||
				   d->conn->error() == AbstractConnection::ReservationLinkError ||
				   d->conn->error() == AbstractConnection::ReservationMapError ||
				   d->conn->error() == AbstractConnection::SocketError)
		{
			status.setChangeReason(Status::ByNetworkError);
		} else if (d->conn->error() == AbstractConnection::MismatchNickOrPassword) {
			Account::setStatus(status);
			config().group("general").setValue("passwd", QString(), Config::Crypted);
			d->lastStatus.setChangeReason(Status::ByAuthorizationFailed);
			setStatus(d->lastStatus);
			return;
		} else if (d->conn->error() == AbstractConnection::RateLimitExceeded) {
			status.setChangeReason(Status::ByNetworkError);
			status.setProperty("reconnectTimeout", 1200);
		} else if (d->conn->error() == AbstractConnection::AnotherClientLogined) {
			// Disable reconnecting
			status.setChangeReason(Status::ByUser);
		} else {
			status.setChangeReason(Status::ByFatalError);
		}
		foreach(IcqContact *contact, d->contacts) {
			OscarStatus status = contact->status();
			status.setType(Status::Offline);
			contact->setStatus(status, false);
			foreach (RosterPlugin *plugin, d->rosterPlugins)
				plugin->statusChanged(contact, status, TLVMap());
		}
	} else if (status == Status::Connecting) {
		emit statusChanged(status, current);
		Account::setStatus(status);
		return;
	} else {
		d->lastStatus = status;
		if (current == Status::Offline) {
			status = Status::createConnecting(status, "icq");
			d->conn->connectToLoginServer(QString());
		} else {
			d->conn->sendStatus(status);
		}
	}
    {
		Config statusCfg = config().group("general/lastStatus");
		statusCfg.setValue("type", d->lastStatus.type());
		statusCfg.setValue("subtype", d->lastStatus.subtype());
		statusCfg.remove("capabilities");
		statusCfg.beginGroup("capabilities");
		QHashIterator<QString, Capability> itr(d->lastStatus.capabilities());
		while (itr.hasNext()) {
			itr.next();
			statusCfg.setValue(itr.key(), itr.value().toString());
		}
		statusCfg.endGroup();
	}
	debug() << status << "with" << Status::connectingGoal(status);
	emit statusChanged(status, current);
	Account::setStatus(status);
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
	if (name == d->name)
		return;
	d->name = name;
	config("general").setValue("nick", name);
}

QString IcqAccount::avatar() const
{
	return d_func()->avatar;
}

void IcqAccount::setAvatar(const QString &avatar)
{
	d_func()->buddyPicture->setAccountAvatar(avatar);
}

ChatUnit *IcqAccount::getUnit(const QString &unitId, bool create)
{
	return getContact(unitId, create);
}

IcqContact *IcqAccount::getContact(const QString &id, bool create, bool forceCreating)
{
	Q_D(IcqAccount);
	IcqContact *contact = 0;
	if (!forceCreating)
		contact = d->contacts.value(id);
	else
		Q_ASSERT(!d->contacts.contains(id));
	if (create && !contact) {
		contact = new IcqContact(id, this);
		d->contacts.insert(id, contact);
		connect(contact, SIGNAL(destroyed()), SLOT(onContactRemoved()));
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

void IcqAccountPrivate::setCapability(const Capability &capability, const QString &type)
{
	if (type.isEmpty()) {
		if (!capability.isNull())
			caps.push_back(capability);
	} else {
		if (!capability.isNull())
			typedCaps.insert(type, capability);
		else
			typedCaps.remove(type);
	}
}

bool IcqAccountPrivate::removeCapability(const Capability &capability)
{
	bool r = caps.removeOne(capability);
	return r;
}

bool IcqAccountPrivate::removeCapability(const QString &type)
{
	bool r = typedCaps.remove(type) > 0;
	return r;
}

void IcqAccount::setCapability(const Capability &capability, const QString &type)
{
	Q_D(IcqAccount);
	d->setCapability(capability, type);
	d->conn->sendUserInfo();
}

bool IcqAccount::removeCapability(const Capability &capability)
{
	Q_D(IcqAccount);
	bool r = d->removeCapability(capability);
	d->conn->sendUserInfo();
	return r;
}

bool IcqAccount::removeCapability(const QString &type)
{
	Q_D(IcqAccount);
	bool r = d->removeCapability(type);
	d->conn->sendUserInfo();
	return r;
}

bool IcqAccount::containsCapability(const Capability &capability) const
{
	Q_D(const IcqAccount);
	if (d->caps.contains(capability))
		return true;
	foreach (const Capability &cap, d->typedCaps) {
		if (cap == capability)
			return true;
	}
	return false;
}

bool IcqAccount::containsCapability(const QString &type) const
{
	Q_D(const IcqAccount);
	return d->typedCaps.contains(type);
}

QList<Capability> IcqAccount::capabilities() const
{
	Q_D(const IcqAccount);
	QList<Capability> caps = d->caps;
	foreach (const Capability &cap, d->typedCaps)
		caps << cap;
	return caps;
}

void IcqAccount::registerRosterPlugin(RosterPlugin *plugin)
{
	Q_D(IcqAccount);
	d->rosterPlugins << plugin;
}

void IcqAccount::setProxy(const QNetworkProxy &proxy)
{
	emit proxyUpdated(proxy);
}

bool IcqAccount::isHtmlEnabled() const
{
	return d_func()->htmlEnabled;
}

void IcqAccount::updateSettings()
{
	Q_D(IcqAccount);
	Config cfg = config("general");
	d->name = cfg.value("nick", QString());
	// AIM contacts support
	if (cfg.value("aimContacts", false))
		setCapability(ICQ_CAPABILITY_AIM_SUPPORT, "aimContacts");
	else
		removeCapability("aimContacts");
	emit settingsUpdated();
}

void IcqAccount::setHtmlEnabled(bool htmlEnabled)
{
	Q_D(IcqAccount);
	if (d->htmlEnabled != htmlEnabled) {
		d->htmlEnabled = htmlEnabled;
		emit htmlEnabledChanged(htmlEnabled);
	}
}

void IcqAccount::onContactRemoved()
{
	Q_D(IcqAccount);
	IcqContact *contact = reinterpret_cast<IcqContact*>(sender());
	QHash<QString, IcqContact *>::iterator itr = d->contacts.begin();
	QHash<QString, IcqContact *>::iterator endItr = d->contacts.end();
	while (itr != endItr) {
		if (*itr == contact) {
			d->contacts.erase(itr);
			break;
		}
		++itr;
	}
	Q_ASSERT(itr != endItr);
}

void IcqAccount::onCookieTimeout()
{
	Q_D(IcqAccount);
	Q_ASSERT(qobject_cast<QTimer*>(sender()));
	QTimer *timer = static_cast<QTimer*>(sender());
	quint64 id = timer->property("cookieId").value<quint64>();
	Q_ASSERT(id != 0);
	Cookie cookie = d->cookies.take(id);
	Q_ASSERT(!cookie.isEmpty());
	QObject *receiver = cookie.receiver();
	const char *member = cookie.member();
	if (receiver && member) {
		const QMetaObject *metaObject = receiver->metaObject();
		int index = metaObject->indexOfMethod(QMetaObject::normalizedSignature(member));
		if (index != -1) {
			metaObject->method(index).invoke(receiver, Qt::AutoConnection, Q_ARG(Cookie, cookie));
		}
	}
	// cookie.unlock(); // Commented out as this cookie is already unlocked
}

} } // namespace qutim_sdk_0_3::oscar


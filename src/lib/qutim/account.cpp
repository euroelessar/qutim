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

#include <QStringBuilder>
#include "account_p.h"
#include "contact.h"
#include "debug.h"
#include "notification.h"
#include "groupchatmanager.h"
#include "inforequest.h"
#include "metaobjectbuilder.h"

#include "libqutim_global.h"

namespace qutim_sdk_0_3
{

Account::Account(const QString &id, Protocol *protocol)
	: Account(*new AccountPrivate(this), protocol)
{
	d_func()->id = id;
}

Account::Account(AccountPrivate &p, Protocol *protocol)
	: MenuController(p, protocol)
{
	Q_D(Account);
	d->protocol = protocol;
	d->status = Status::instance(Status::Offline, protocol->id().toLatin1());
	d->userStatus = d->status;

	// Hack for authorization failed notification,
	// move once to some plugin
	connect(this, &Account::disconnected, this, [this] (Status::ChangeReason reason) {
		if (reason == Status::ByAuthorizationFailed) {
			NotificationRequest request(Notification::System);
			request.setText(QT_TRANSLATE_NOOP("Account", "Authorization failed"));
			request.send();
		}
	});
}

Account::~Account()
{
	//		ConfigGroup data = config("properties");
	//		foreach(const QByteArray &name, dynamicPropertyNames())
	//			data.setValue(name, property(name));
	//		data.sync();
}

QString Account::id() const
{
	return d_func()->id;
}

QString Account::name() const
{
	return d_func()->id;
}

Config Account::config()
{
	Q_D(Account);
	QStringList paths;
	paths << d->protocol.data()->id() % QLatin1Char('.') % d->id % QLatin1Literal("/account");
	paths << d->protocol.data()->id();
	return Config(paths);
}

ConfigGroup Account::config(const QString &name)
{
	return config().group(name);
}

Status Account::status() const
{
	return d_func()->status;
}

Protocol *Account::protocol()
{
	return d_func()->protocol.data();
}

const Protocol *Account::protocol() const
{
	return d_func()->protocol.data();
}

Account::State Account::state() const
{
	return d_func()->state;
}

void Account::connectToServer()
{
	Q_D(Account);

	switch (d->state) {
	case Disconnected:
		setState(Connecting);
		doConnectToServer();
		break;
	case Connecting:
		break;
	case Connected:
		break;
	case Disconnecting:
		setState(Connecting);
		doConnectToServer();
		break;
	}
}

void Account::disconnectFromServer()
{
	Q_D(Account);

	switch (d->state) {
	case Disconnected:
		break;
	case Connecting:
		setState(Disconnecting);
		doDisconnectFromServer();
		break;
	case Connected:
		setState(Disconnecting);
		doDisconnectFromServer();
		break;
	case Disconnecting:
		break;
	}
}

void Account::setState(Account::State state, Status::ChangeReason reason)
{
	Q_D(Account);

	if (d->state != state) {
		d->state = state;
		emit stateChanged(state);

		if (state == Connected)
			emit connected();
		else if (state == Disconnected)
			emit disconnected(reason);

		d->updateStatus();
	}
}

void AccountPrivate::updateStatus()
{
	const QByteArray protocol = q_func()->protocol()->id().toLatin1();

	switch (state) {
	case Account::Connecting:
		setStatus(Status::createConnecting(userStatus, protocol));
		break;
	case Account::Disconnected:
		setStatus(Status::instance(Status::Offline, protocol));
		break;
	case Account::Connected:
	case Account::Disconnecting:
		q_func()->doStatusChange(userStatus);
		setStatus(userStatus);
		break;
	}
}

void AccountPrivate::setStatus(const Status &newStatus)
{
	Status oldStatus = status;
	status = newStatus;
	emit q_func()->statusChanged(newStatus, oldStatus);
}

void Account::setUserStatus(const Status &userStatus)
{
	Q_D(Account);

	Status oldUserStatus = d->userStatus;
	d->userStatus = userStatus;
	emit userStatusChanged(userStatus, oldUserStatus);

	const bool oldOffline = oldUserStatus == Status::Offline;
	const bool newOffline = userStatus == Status::Offline;

	if (oldOffline && !newOffline) {
		// Go online
		if (state() == Disconnected)
			connectToServer();
	} else if (!oldOffline && newOffline) {
		// Go offline
		if (state() == Connected || state() == Connecting)
			disconnectFromServer();
	}

	d->updateStatus();
}

Status Account::userStatus() const
{
	return d_func()->userStatus;
}

ChatUnit *Account::getUnitForSession(ChatUnit *unit)
{
	return unit;
}

QVariantMap Account::parameters() const
{
	QVariantMap result;
	const_cast<Account*>(this)->virtual_hook(ReadParametersHook, &result);
	return result;
}

QStringList Account::updateParameters(const QVariantMap &parameters)
{
	UpdateParametersArgument argument = { parameters, QStringList() };
	virtual_hook(UpdateParametersHook, &argument);
	return argument.reconnectionRequired;
}

QObject *Account::feature(const QMetaObject *meta)
{
	Q_D(Account);

	QByteArray name = MetaObjectBuilder::info(meta, "Feature");

	if (name.isEmpty()) {
		qWarning() << "Feature name is missed at class:" << meta->className();
		return nullptr;
	}

	AccountInterface result = d->interfaces.value(name);
	qDebug() << "Found" << result.object << "as" << name << "for" << this;
	return result.object;
}

void Account::setFeature(const QMetaObject *meta, QObject *feature)
{
	Q_D(Account);

	QByteArray name = MetaObjectBuilder::info(meta, "Feature");

	if (name.isEmpty()) {
		qWarning() << "Feature name is missed at class:" << meta->className();
		return;
	}

	qDebug() << "Setting" << feature << "as" << name << "for" << this;

	AccountInterface &current = d->interfaces[name];
	std::swap(current.object, feature);

	emit featureChanged(name, current.object);
}

GroupChatManager *Account::groupChatManager()
{
	return feature<GroupChatManager>();
}

ContactsFactory *Account::contactsFactory()
{
	return feature<ContactsFactory>();
}

InfoRequestFactory *Account::infoRequestFactory()
{
	return feature<InfoRequestFactory>();
}

void Account::resetGroupChatManager(GroupChatManager *manager)
{
	setFeature<GroupChatManager>(manager);
}

void Account::setContactsFactory(ContactsFactory *factory)
{
	setFeature<ContactsFactory>(factory);
}

void Account::setInfoRequestFactory(InfoRequestFactory *factory)
{
	setFeature<InfoRequestFactory>(factory);
}

void Account::setEffectiveStatus(const Status &status)
{
	d_func()->setStatus(status);
}

}


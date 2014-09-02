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

namespace qutim_sdk_0_3
{

Account::Account(const QString &id, Protocol *protocol)
	: MenuController(*new AccountPrivate(this), protocol)
{
	Q_D(Account);
	d->protocol = protocol;
	d->id = id;
}

Account::Account(AccountPrivate &p, Protocol *protocol)
	: MenuController(p, protocol)
{
	Q_D(Account);
	d->protocol = protocol;
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

void Account::setStatus(Status status)
{
	Q_D(Account);

	Status::ChangeReason reason = status.changeReason();

	switch(reason) {
	case Status::ByUser:
	case Status::ByIdle:
	case Status::ByNetworkError:
	case Status::ByFatalError:
	case Status::ByQuit:
		break;
	case Status::ByAuthorizationFailed: {
		NotificationRequest request(Notification::System);
		request.setText(QT_TRANSLATE_NOOP("Account", "Authorization failed"));
		request.send();
		break;
	}
	}

	Status old = d->status;
	d->status = status;
	emit statusChanged(status,old);
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

QObject *Account::interface(const QMetaObject *meta)
{
	Q_D(Account);

	QByteArray name = MetaObjectBuilder::info(meta, "Interface");

	if (name.isEmpty()) {
		qWarning() << "Interface name is missed at class:" << meta->className();
		return nullptr;
	}

	auto it = d->interfaces.find(name);
	if (it != d->interfaces.end()) {
		const AccountInterface &interface = it->second;
		return interface.object.get();
	}

	return nullptr;
}

void Account::setInterface(const QMetaObject *meta, QObject *interface)
{
	Q_D(Account);

	std::unique_ptr<QObject> tmp(interface);
	QByteArray name = MetaObjectBuilder::info(meta, "Interface");

	if (name.isEmpty()) {
		qWarning() << "Interface name is missed at class:" << meta->className();
		return;
	}

	auto it = d->interfaces.find(name);
	if (it == d->interfaces.end()) {
		it = d->interfaces.emplace(name, AccountInterface()).first;
	}

	AccountInterface &current = it->second;
	std::swap(current.object, tmp);

	emit interfaceChanged(name, current.object.get());
}

GroupChatManager *Account::groupChatManager()
{
	return interface<GroupChatManager>();
}

ContactsFactory *Account::contactsFactory()
{
	return interface<ContactsFactory>();
}

InfoRequestFactory *Account::infoRequestFactory()
{
	return interface<InfoRequestFactory>();
}

void Account::resetGroupChatManager(GroupChatManager *manager)
{
	setInterface<GroupChatManager>(manager);
}

void Account::setContactsFactory(ContactsFactory *factory)
{
	setInterface<ContactsFactory>(factory);
}

void Account::setInfoRequestFactory(InfoRequestFactory *factory)
{
	setInterface<InfoRequestFactory>(factory);
}

}


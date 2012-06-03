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

#include "contactlist.h"
#include <QDeclarativeContext>
#include <QDeclarativeEngine>
#include <qutim/thememanager.h>
#include "notificationmanagerwrapper.h"
#include "contactlistmodel.h"

namespace MeegoIntegration
{
using namespace qutim_sdk_0_3;

AccountList allAccounts()
{
	AccountList accounts;
	foreach (Protocol *protocol, Protocol::all())
		accounts << protocol->accounts();
	return accounts;
}

ContactList::ContactList()
{
	init();
	NotificationManagerWrapper::init();
	foreach (Protocol *protocol, Protocol::all()) {
		connect(protocol, SIGNAL(accountCreated(qutim_sdk_0_3::Account*)),
		        SLOT(onAccountAdded(qutim_sdk_0_3::Account*)));
		connect(protocol, SIGNAL(accountRemoved(qutim_sdk_0_3::Account*)),
		        SLOT(onAccountRemoved(qutim_sdk_0_3::Account*)));
		m_accounts << protocol->accounts();
	}
	m_protocols = Protocol::all().values();
}

void ContactList::init()
{
	qmlRegisterType<Protocol>();
	qmlRegisterType<Account>();
	qmlRegisterType<ChatUnit>();
	qmlRegisterType<Contact>();
	qmlRegisterType<ContactListModel>("org.qutim", 0, 3, "ContactListModel");
	qmlRegisterUncreatableType<StatusWrapper>("org.qutim", 0, 3, "Status", "It's not a creatable type");
}

StatusWrapper::Type ContactList::status() const
{
	return static_cast<StatusWrapper::Type>(m_globalStatus.type());
}

void ContactList::setStatus(StatusWrapper::Type type)
{
	m_globalStatus.setType(static_cast<Status::Type>(type));
	foreach (Account *account, m_accounts)
		account->setStatus(m_globalStatus);
	emit statusChanged(type);
}

QDeclarativeListProperty<qutim_sdk_0_3::Account> ContactList::accounts()
{
	QDeclarativeListProperty<qutim_sdk_0_3::Account> list(this, m_accounts);
	list.append = NULL;
	list.clear = NULL;
	return list;
}

QDeclarativeListProperty<qutim_sdk_0_3::Protocol> ContactList::protocols()
{
	QDeclarativeListProperty<qutim_sdk_0_3::Protocol> list(this, m_protocols);
	list.append = NULL;
	list.clear = NULL;
	return list;
}

QString ContactList::statusName(const QVariant &type)
{
	return Status(static_cast<Status::Type>(type.toInt())).name().toString();
}

QUrl ContactList::statusUrl(const QVariant &type)
{
	return statusUrl(type, QLatin1String("icon-m"));
}

QString ContactList::statusIcon(const QVariant &type)
{
	return statusIcon(type, QLatin1String("icon-m"));
}

QUrl ContactList::statusUrl(const QVariant &type, const QString &subtype)
{
	return QUrl::fromUserInput(QLatin1String("image://theme/") + statusIcon(type, subtype));
}

QString ContactList::statusIcon(const QVariant &type, const QString &subtype)
{
	QString iconName = subtype + QLatin1String("-presence-");
	int statusType = type.toInt();
	if (type.canConvert<Status>())
		statusType = type.value<Status>().type();
	switch (statusType) {
	case Status::Online:
	case Status::FreeChat:
		iconName += QLatin1String("online");
		break;
	case Status::Away:
		iconName += QLatin1String("away");
		break;
	case Status::NA:
	case Status::DND:
		iconName += QLatin1String("busy");
		break;
	case Status::Offline:
		iconName += QLatin1String("offline");
		break;
	default:
	case Status::Connecting:
	case Status::Invisible:
		iconName += QLatin1String("unknown");
		break;
	}
	return iconName;
}

void ContactList::onAccountAdded(qutim_sdk_0_3::Account *account)
{
	m_accounts << account;
	emit accountsChanged(accounts());
}

void ContactList::onAccountRemoved(qutim_sdk_0_3::Account *account)
{
	m_accounts.removeAll(account);
	emit accountsChanged(accounts());
}
}


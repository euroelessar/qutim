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

#include "proxyaccount.h"
#include "proxycontact.h"
#include <qutim/event.h>

ProxyAccount::ProxyAccount(Account *account) :
	Account(account->id(), account->protocol()), m_account(account)
{
	connect(account, SIGNAL(contactCreated(qutim_sdk_0_3::Contact*)),
			SIGNAL(contactCreated(qutim_sdk_0_3::Contact*)));
	connect(account, SIGNAL(conferenceCreated(qutim_sdk_0_3::Conference*)),
			SIGNAL(conferenceCreated(qutim_sdk_0_3::Conference*)));
	connect(account, SIGNAL(nameChanged(QString,QString)),
			SIGNAL(nameChanged(QString,QString)));

	connect(account, &Account::userStatusChanged, this, &Account::setUserStatus);
	connect(account, &Account::stateChanged, this, [this] (State state) {
		setState(state);
	});

	setUserStatus(m_account->status());
	setState(m_account->state());
}

QString ProxyAccount::name() const
{
	return m_account->name();
}

ChatUnit *ProxyAccount::getUnitForSession(ChatUnit *unit)
{
	if (ProxyContact *contact = qobject_cast<ProxyContact*>(unit))
		return contact->conference();
	return m_account->getUnitForSession(unit);
}

ChatUnit *ProxyAccount::getUnit(const QString &unitId, bool create)
{
	return m_account->getUnit(unitId, create);
}

bool ProxyAccount::event(QEvent *ev)
{
	if (ev->type() == Event::eventType()) {
		Event *event = static_cast<Event*>(ev);
		static quint16 realAccountRequestEvent = Event::registerType("real-account-request");
		if (event->id == realAccountRequestEvent) {
			event->args[0] = qVariantFromValue<Account*>(m_account);
			event->accept();
			return true;
		}
	}
	return Account::event(ev);
}

void ProxyAccount::doConnectToServer()
{
}

void ProxyAccount::doDisconnectFromServer()
{
}

void ProxyAccount::doStatusChange(const Status &status)
{
	Q_UNUSED(status);
}

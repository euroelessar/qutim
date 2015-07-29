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
#ifndef IDLESTATUSCHANGER_H
#define IDLESTATUSCHANGER_H

#include <qutim/libqutim_global.h>
#include <qutim/account.h>

using namespace qutim_sdk_0_3;

namespace Core
{
class IdleStatusChanger : public QObject
{
	Q_OBJECT
	Q_CLASSINFO("Service", "AutoAway")
	Q_CLASSINFO("Uses", "Idle")
public:
	explicit IdleStatusChanger();

public slots:
	void onIdle(int secs);

private:
	void refillAccounts();
	void reloadSettings();

	enum State
	{
		Active,
		Away,
		Inactive
	};
	qutim_sdk_0_3::ConfigValue<bool> m_awayEnabled;
	qutim_sdk_0_3::ConfigValue<bool> m_naEnabled;
	qutim_sdk_0_3::ConfigValue<int> m_awaySecs;
	qutim_sdk_0_3::ConfigValue<int> m_naSecs;

	Status m_naStatus;
	Status m_awayStatus;

	qutim_sdk_0_3::ConfigValue<QString> m_naStatusText;
	qutim_sdk_0_3::ConfigValue<QString> m_awayStatusText;

	State m_state;
	QList<QPointer<Account> > m_accounts;
	QList<Status> m_statuses;
};
}

#endif // IDLESTATUSCHANGER_H


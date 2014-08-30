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
#include "idle-global.h"
#include "idlestatuschanger.h"
#include <qutim/accountmanager.h>
#include <qutim/config.h>
#include <qutim/status.h>
#include <qutim/settingslayer.h>
#include <qutim/icon.h>
#include <qutim/servicemanager.h>
#include <QDebug>

namespace Core
{
IdleStatusChanger::IdleStatusChanger() :
	m_awayStatus(Status::Away), m_naStatus(Status::NA)
{
	QObject *idle = ServiceManager::getByName("Idle");
	reloadSettings();
	m_state = Active;
	connect(idle, SIGNAL(secondsIdle(int)), this, SLOT(onIdle(int)));
	SettingsItem *settings = new QmlSettingsItem(
                QStringLiteral("idlestatuschanger"),
				Settings::General,
				Icon("user-away-extended"),
				QT_TRANSLATE_NOOP("AutoAway", "Auto-away"));
    settings->connect(SIGNAL(saved()), this, SLOT(reloadSettings()));
	Settings::registerItem(settings);

	m_awayStatus.setChangeReason(Status::ByIdle);
	m_naStatus.setChangeReason(Status::ByIdle);
}

void IdleStatusChanger::refillAccounts()
{
	foreach (Account *account, AccountManager::instance()->accounts()) {
		if (m_accounts.contains(account)
				|| account->status() == Status::Offline
				|| account->status() == Status::Invisible
				|| account->status() == Status::DND
				|| account->status() == Status::NA)
			continue;
		m_accounts.append(account);
		m_statuses.append(account->status());
	}
}

void IdleStatusChanger::onIdle(int secs)
{
	if ((m_awayEnabled?m_state == Away:1)
			&& secs > m_naSecs
			&& m_naEnabled) {
		refillAccounts();
		foreach (const QPointer<Account> &acc, m_accounts) {
			if (acc)
				acc.data()->setStatus(m_naStatus);
		}
		m_state = Inactive;
	} else if (m_state == Active
			   && secs > m_awaySecs
			   && m_awayEnabled) {
		refillAccounts();
		foreach (const QPointer<Account> &acc, m_accounts) {
			if (acc)
				acc.data()->setStatus(m_awayStatus);
		}
		m_state = Away;
	} else if (m_state != Active && secs < m_awaySecs) {
		for (int i = 0; i < m_accounts.size(); i++) {
			if (m_accounts.at(i))
				m_accounts.at(i).data()->setStatus(m_statuses.at(i));
		}
		m_accounts.clear();
		m_statuses.clear();
		m_state = Active;
	}
}

void IdleStatusChanger::reloadSettings()
{
	Config conf(AA_CONFIG_GROUP);
	m_awayEnabled = conf.value("away-enabled", true);
	m_naEnabled   = conf.value("na-enabled",   true);
	m_awaySecs = conf.value("away-secs", AWAY_DEF_SECS);
	m_naSecs   = conf.value("na-secs",   NA_DEF_SECS);
	m_awayStatus.setText(conf.value("away-text", ""));
	m_naStatus.  setText(conf.value("na-text",   ""));
}
}


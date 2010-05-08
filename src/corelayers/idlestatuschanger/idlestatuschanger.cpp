#include "idlestatuschanger.h"
#include "libqutim/protocol.h"
#include "src/modulemanagerimpl.h"
#include <QDebug>

namespace Core
{
static CoreSingleModuleHelper<IdleStatusChanger> history_static(
	QT_TRANSLATE_NOOP("Plugin", "Idle status changer"),
	QT_TRANSLATE_NOOP("Plugin", "Change status of accounts by idle")
);

IdleStatusChanger::IdleStatusChanger() :
		m_awayStatus(Status::Away), m_naStatus(Status::NA)
{
	QObject *idle = getService("Idle");

	m_awaySecs = 30;
	m_naSecs   = 120;
	m_state = Active;
	connect(idle, SIGNAL(secondsIdle(int)), this, SLOT(onIdle(int)));
}

void IdleStatusChanger::refillAccounts()
{
	foreach (Protocol *proto, allProtocols()) {
		foreach (Account *acc, proto->accounts()) {
			if (m_accounts.contains(acc) || acc->status() == Status::Offline)
				continue;
			m_accounts.append(acc);
			m_statuses.append(acc->status());
		}
	}
}

void IdleStatusChanger::onIdle(int secs)
{
	if (m_state == Away && secs > m_naSecs) {
		refillAccounts();
		foreach (Account *acc, m_accounts) {
			if (acc)
				acc->setStatus(m_naStatus);
		}
		m_state = Inactive;
	} else if (m_state == Active && secs > m_awaySecs) {
		refillAccounts();
		foreach (Account *acc, m_accounts) {
			if (acc)
				acc->setStatus(m_awayStatus);
		}
		m_state = Away;
	} else if (m_state != Active && secs < m_awaySecs) {
		for (int i = 0; i < m_accounts.size(); i++) {
			if (m_accounts.at(i))
				m_accounts.at(i)->setStatus(m_statuses.at(i));
		}
		m_accounts.clear();
		m_statuses.clear();
		m_state = Active;
	}
}
}

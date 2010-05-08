#ifndef IDLESTATUSCHANGER_H
#define IDLESTATUSCHANGER_H

#include "libqutim/libqutim_global.h"
#include "libqutim/account.h"

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
	enum State
	{
		Active,
		Away,
		Inactive
	};
	State m_state;
	int m_awaySecs;
	Status m_awayStatus;
	int m_naSecs;
	Status m_naStatus;
	QList<QPointer<Account> > m_accounts;
	QList<Status> m_statuses;
};
}

#endif // IDLESTATUSCHANGER_H

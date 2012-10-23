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

#ifndef PROXYACCOUNT_H
#define PROXYACCOUNT_H

#include <qutim/protocol.h>
#include <qutim/account.h>

using namespace Ureen;

class ProxyAccount : public Account
{
	Q_OBJECT
public:
	ProxyAccount(Account *account);
	virtual QString name() const;
	virtual void setStatus(Status status);
	virtual ChatUnit *getUnitForSession(ChatUnit *unit);
	virtual ChatUnit *getUnit(const QString &unitId, bool create = false);
private slots:
	void onStatusChanged(const Ureen::Status &current, const Ureen::Status &previous);
protected:
	bool event(QEvent *ev);
private:
	Account *m_account;
};

#endif // PROXYACCOUNT_H


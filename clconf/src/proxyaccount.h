/****************************************************************************
 *  proxyaccount.h
 *
 *  Copyright (c) 2010 by Prokhin Alexey <alexey.prokhin@yandex.ru>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
 *****************************************************************************/

#ifndef PROXYACCOUNT_H
#define PROXYACCOUNT_H

#include <qutim/protocol.h>
#include <qutim/account.h>

using namespace qutim_sdk_0_3;

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
	void onStatusChanged(const qutim_sdk_0_3::Status &current, const qutim_sdk_0_3::Status &previous);
protected:
	bool event(QEvent *ev);
private:
	Account *m_account;
};

#endif // PROXYACCOUNT_H

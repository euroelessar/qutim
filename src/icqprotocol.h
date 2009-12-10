/****************************************************************************
 *  icqprotocol.h
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#ifndef ICQPROTOCOL_H
#define ICQPROTOCOL_H

#include <qutim/protocol.h>

namespace Icq {

using namespace qutim_sdk_0_3;

struct IcqProtocolPrivate;
class IcqProtocol;
struct IcqAccWizardPrivate;

class IcqAccountCreationWizard : public AccountCreationWizard
{
	Q_OBJECT
public:
	IcqAccountCreationWizard(IcqProtocol *protocol);
	~IcqAccountCreationWizard();
	QList<QWizardPage *> createPages(QWidget *parent);
	void finished();
private:
	QScopedPointer<IcqAccWizardPrivate> p;
};

class IcqProtocol : public Protocol
{
	Q_OBJECT
	Q_CLASSINFO("Protocol", "icq")
public:
	IcqProtocol();
	virtual ~IcqProtocol();
	static inline IcqProtocol *instance() { if(!self) qWarning("IcqProtocol isn't created"); return self; }
	virtual AccountCreationWizard *accountCreationWizard();
	virtual QList<Account *> accounts() const;
	virtual Account *account(const QString &id) const;
protected:
	void loadAccounts();
private slots:
	void onStatusActionPressed();
private:
	friend class IcqAccountCreationWizard;
	QScopedPointer<IcqProtocolPrivate> p;
	static IcqProtocol *self;
};

} // namespace Icq

#endif // ICQPROTOCOL_H

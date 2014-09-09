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
#ifndef AUTHORIZATION_P_H
#define AUTHORIZATION_P_H

#include "snachandler.h"
#include "feedbag.h"
#include <qutim/actiongenerator.h>

namespace qutim_sdk_0_3 {

namespace oscar {

class AuthorizeActionGenerator : public ActionGenerator
{
public:
	AuthorizeActionGenerator();
protected:
	virtual void createImpl(QAction *action,QObject *obj) const;
};

class Authorization : public QObject, public SNACHandler, public FeedbagItemHandler
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::oscar::SNACHandler qutim_sdk_0_3::oscar::FeedbagItemHandler)
	Q_CLASSINFO("DependsOn", "qutim_sdk_0_3::oscar::IcqProtocol")
public:
    Authorization();
	static Authorization *instance() { Q_ASSERT(self); return self; }
protected:
	void handleSNAC(AbstractConnection *conn, const SNAC &snac);
	bool handleFeedbagItem(Feedbag *feedbag, const FeedbagItem &item, Feedbag::ModifyType type, FeedbagError error);
private slots:
	void onSendRequestClicked(QObject *object);
	void onGrantAuthClicked(QObject *object);
private:
	void onAuthChanged(IcqContact *contact, bool auth);
private:
	static Authorization *self;
	ActionGenerator *m_authActionGen;
};

} } // namespace qutim_sdk_0_3::oscar

#endif // AUTHORIZATION_P_H



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

#ifndef ROSTER_P_H
#define ROSTER_P_H

#include "oscarroster.h"
#include "snachandler.h"
#include "icqcontact.h"
#include "feedbag.h"
#include "oscarstatus.h"

namespace qutim_sdk_0_3 {

namespace oscar {

class OscarConnection;

class Roster : public QObject, public SNACHandler, public FeedbagItemHandler
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::oscar::SNACHandler qutim_sdk_0_3::oscar::FeedbagItemHandler)
	Q_CLASSINFO("DependsOn", "qutim_sdk_0_3::oscar::IcqProtocol")
public:
	Roster();
	
	QStringList readTags(const FeedbagItem &item);
	
protected:
	bool handleFeedbagItem(Feedbag *feedbag, const FeedbagItem &item, Feedbag::ModifyType type, FeedbagError error);
	void handleAddModifyCLItem(IcqAccount *account, const FeedbagItem &item, Feedbag::ModifyType type);
	void handleRemoveCLItem(IcqAccount *account, const FeedbagItem &item);
	void removeContact(IcqContact *contact);
	void removeContactFromGroup(IcqContact *contact, quint16 groupId);
	virtual void handleSNAC(AbstractConnection *conn, const SNAC &snac);
	void handleUserOnline(IcqAccount *account, const SNAC &snac);
	void handleUserOffline(IcqAccount *account, const SNAC &snac);
private slots:
	void reloadingStarted();
	void loginFinished();
	void accountAdded(qutim_sdk_0_3::Account *account);
private:
	void setStatus(IcqContact *contact, OscarStatus &status, const TLVMap &tlvs);
};

} } // namespace qutim_sdk_0_3::oscar

#endif // ROSTER_P_H


/****************************************************************************
 *  roster_p.h
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *                        Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#ifndef ROSTER_P_H
#define ROSTER_P_H

#include "roster.h"
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
public:
	Roster();
protected:
	bool handleFeedbagItem(Feedbag *feedbag, const FeedbagItem &item, Feedbag::ModifyType type, FeedbagError error);
	void handleAddModifyCLItem(IcqAccount *account, const FeedbagItem &item);
	void handleRemoveCLItem(IcqAccount *account, const FeedbagItem &item);
	void loadTagsFromFeedbag(IcqContact *contact);
	void removeContact(IcqContact *contact);
	void removeContactFromGroup(IcqContact *contact, quint16 groupId);
	QStringList readTags(const FeedbagItem &item);
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

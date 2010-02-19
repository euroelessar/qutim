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
#include <QQueue>
#include "snachandler.h"
#include "icqcontact.h"
#include "feedbag.h"
#include "oscarstatus.h"

namespace Icq
{

class OscarConnection;
class SsiHandler;

class PrivateListActionHandler : public QObject
{
	Q_OBJECT
public slots:
	void onModifyPrivateList();
};

class SsiHandler : public FeedbagItemHandler
{
	Q_OBJECT
public:
	SsiHandler(IcqAccount *account, QObject *parent = 0);
protected:
	bool handleFeedbagItem(Feedbag *feedbag, const FeedbagItem &item, Feedbag::ModifyType type, FeedbagError error);
	void handleAddModifyCLItem(const FeedbagItem &item, Feedbag::ModifyType type);
	void handleRemoveCLItem(const FeedbagItem &item);
	void removeContact(IcqContact *contact);
	void removeContactFromGroup(IcqContact *contact, quint16 groupId);
	QStringList readTags(const FeedbagItem &item);
private:
	IcqAccount *m_account;
};

class Roster : public SNACHandler
{
	Q_OBJECT
public:
	Roster(IcqAccount *account);
protected:
	virtual void handleSNAC(AbstractConnection *conn, const SNAC &snac);
	void handleUserOnline(const SNAC &snac);
	void handleUserOffline(const SNAC &snac);
private slots:
	void statusChanged(qutim_sdk_0_3::Status status);
	void loginFinished();
private:
	void setStatus(IcqContact *contact, OscarStatus &status, const TLVMap &tlvs);
	IcqAccount *m_account;
	OscarConnection *m_conn;
	SsiHandler *m_ssiHandler;
	bool firstPacket;
};

} // namespace Icq

#endif // ROSTER_P_H

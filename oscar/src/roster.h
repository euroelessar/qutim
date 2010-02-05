/****************************************************************************
 *  roster.h
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#ifndef ROSTER_H
#define ROSTER_H

#include <QQueue>
#include "snachandler.h"
#include "icqcontact.h"
#include "feedbag.h"

namespace Icq
{

class OscarConnection;
class SsiHandler;

enum Visibility
{
	AllowAllUsers    = 1,
	BlockAllUsers    = 2,
	AllowPermitList  = 3,
	BlockDenyList    = 4,
	AllowContactList = 5
};

class Roster : public SNACHandler
{
	Q_OBJECT
public:
	Roster(IcqAccount *account);
	IcqContact *contact(const QString &uin);
	const QHash<QString, IcqContact *> &contacts() const;
	void sendAuthResponse(const QString &id, const QString &message, bool auth = true);
	void sendAuthRequest(const QString &id, const QString &message);
	IcqContact *sendAddContactRequest(const QString &contactId, const QString &contactName, quint16 groupId);
	void setVisibility(Visibility visibility);
protected:
	virtual void handleSNAC(AbstractConnection *conn, const SNAC &snac);
private slots:
	void loginFinished();
private:
	void handleUserOnline(const SNAC &snac);
	void handleUserOffline(const SNAC &snac);
	IcqAccount *m_account;
	OscarConnection *m_conn;
	SsiHandler *m_ssiHandler;
};

} // namespace Icq

#endif // ROSTER_H

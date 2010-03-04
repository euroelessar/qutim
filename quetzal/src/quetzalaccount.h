/****************************************************************************
 *  quetzalaccount.h
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

#ifndef QUETZALACCOUNT_H
#define QUETZALACCOUNT_H

#include <purple.h>
#include <qutim/account.h>
#include "quetzalcontact.h"

using namespace qutim_sdk_0_3;

class QuetzalProtocol;

class QuetzalAccount : public Account
{
	Q_OBJECT
public:
	QuetzalAccount(const QString &id, QuetzalProtocol *protocol);
	virtual ChatUnit *getUnit(const QString &unitId, bool create = false);
	void createNode(PurpleBlistNode *node);
	void load(Config cfg);
	void save();
	void save(QuetzalContact *contact);
	void remove(QuetzalContact *contact);
	void save(PurpleChat *chat);
	void remove(PurpleChat *chat);
	void addChatUnit(ChatUnit *unit);
	void removeChatUnit(ChatUnit *unit);
	virtual void setStatus(Status status);
	void setStatusChanged(PurpleStatus *status);
	void requestPassword(PurpleRequestFieldsCb okCb, PurpleRequestFieldsCb cancelCb, void *userData);
	Q_INVOKABLE int sendRawData(const QByteArray &data);
signals:

private slots:
	void showJoinGroupChat();
	void onPasswordEntered(const QString &password, bool remember);
	void onPasswordRejected();
private:
	QHash<QString, ChatUnit *> m_units;
	QHash<QString, QuetzalContact *> m_contacts;
	PurpleAccount *m_account;
	bool m_is_loading;
};

Q_DECLARE_METATYPE(PurpleAccount*)

#endif // QUETZALACCOUNT_H

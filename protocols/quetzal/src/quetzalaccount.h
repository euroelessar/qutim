/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef QUETZALACCOUNT_H
#define QUETZALACCOUNT_H

#include <purple.h>
#include <qutim/account.h>
#include "quetzalcontact.h"
#include <qutim/rosterstorage.h>
#include <QBasicTimer>

using namespace qutim_sdk_0_3;

class QuetzalProtocol;
struct QuetzalAccountPasswordInfo;

class QuetzalContactsFactory : public qutim_sdk_0_3::ContactsFactory
{
public:
	QuetzalContactsFactory(QuetzalAccount *account);
	virtual Contact *addContact(const QString &id, const QVariantMap &data);
	virtual void serialize(Contact *contact, QVariantMap &data);
private:
	QuetzalAccount *m_account;
};

class QuetzalAccount : public Account
{
	Q_OBJECT
public:
	QuetzalAccount(PurpleAccount *account, QuetzalProtocol *protocol);
	QuetzalAccount(const QString &id, QuetzalProtocol *protocol);
	~QuetzalAccount();
	QuetzalContact *createContact(const QString &id);
	virtual ChatUnit *getUnitForSession(ChatUnit *unit);
	virtual ChatUnit *getUnit(const QString &unitId, bool create = false);
	void createNode(PurpleBlistNode *node);
	void load(Config cfg);
	void save();
	void save(PurpleBuddy *buddy);
	void remove(PurpleBuddy *buddy);
	void save(PurpleChat *chat);
	void remove(PurpleChat *chat);
	void addChatUnit(ChatUnit *unit);
	void removeChatUnit(ChatUnit *unit);
	virtual void setStatus(Status status);
	void setStatusChanged(PurpleStatus *status);
	void handleSigningOn();
	void handleSignedOn();
	void handleSignedOff();
	QObject *requestPassword(PurpleRequestFields *fields, PurpleRequestFieldsCb okCb,
							 PurpleRequestFieldsCb cancelCb, void *userData);
	PurpleAccount *purple();
	Q_INVOKABLE int sendRawData(const QByteArray &data);
	
	void timerEvent(QTimerEvent *);
protected:
	void fillStatusActions();
	MenuController::ActionList dynamicActions() const;
	void fillPassword(const QuetzalAccountPasswordInfo &info, const QString &password);

private slots:
	void showJoinGroupChat();
	void onPasswordEntered(const QString &password, bool remember);
	void onPasswordRejected();
private:
	QHash<QString, ChatUnit *> m_units;
	QHash<QString, QuetzalContact *> m_contacts;
	PurpleAccount *m_account;
	bool m_isLoading;
	QBasicTimer m_chatTimer;
	friend class QuetzalContactsFactory;
};

Q_DECLARE_METATYPE(PurpleAccount*)

#endif // QUETZALACCOUNT_H


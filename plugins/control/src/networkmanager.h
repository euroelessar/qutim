/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef CONTROL_NETWORKMANAGER_H
#define CONTROL_NETWORKMANAGER_H

#include <QNetworkAccessManager>
#include <QDateTime>
#include <QUrl>
#include <QBasicTimer>
#include <qutim/account.h>
#include <qutim/contact.h>

namespace Control {

class AccountId
{
public:
	AccountId() {}
	AccountId(qutim_sdk_0_3::Account *account);
	AccountId(const QString &id, const QString &protocol);
	
	bool isEmpty() const { return id.isEmpty(); }
	bool operator ==(const AccountId &o) const
	{ return id == o.id && protocol == o.protocol; }
	
	QString id;
	QString protocol;
};

class Action
{
public:
	enum Type {
		GetAccounts,
		AddAccount,
		RemoveAccount,
		AddContact,
		UpdateContact,
		RemoveContact,
		Message
	} type;
	AccountId account;
	Action *prev;
	Action *next;

protected:
	Action(Type type) : type(type), prev(0), next(0) {}
};

class AccountAction : public Action
{
public:
	AccountAction(Type type) : Action(type) {}
};

class ContactAction : public Action
{
public:
	ContactAction(Type type, qutim_sdk_0_3::Contact *contact)
	    : Action(type), id(contact->id()), name(contact->name()),
	      groups(contact->tags())
	{
		account = AccountId(contact->account());
	}
	QString id;
	QString name;
	QStringList groups;
};

class MessageAction : public Action
{
public:
	MessageAction() : Action(Message) {}
	QString contact;
	QDateTime time;
	QString text;
	bool incoming;
};

class ActionList
{
public:
	ActionList();
	~ActionList();
	
	Action *first();
	Action *last();
	bool isEmpty() const;
	bool hasSingleElement() const;
	int count() const;
	
	inline void operator <<(Action *action) { append(action); }
	void append(Action *action);
	void append(ActionList &o);
	void prepend(Action *action);
	void prepend(ActionList &o);
	void remove(Action *action);
private:
	Q_DISABLE_COPY(ActionList)
	Action *m_first;
	Action *m_last;
};

class NetworkManager : public QNetworkAccessManager
{
	Q_OBJECT
public:
	explicit NetworkManager(QObject *parent = 0);
	
	void timerEvent(QTimerEvent *ev);
	
	void addAccount(const QString &protocol, const QString &id);
	void removeAccount(const QString &protocol, const QString &id);
	void addContact(qutim_sdk_0_3::Contact *contact);
	void removeContact(qutim_sdk_0_3::Contact *contact);
	void updateContact(qutim_sdk_0_3::Contact *contact);
	void sendMessage(const qutim_sdk_0_3::Message &message);
	
	QNetworkReply *post(const QUrl &url, const QByteArray &body);
	QNetworkReply *get(const QUrl &url);
	
protected slots:
	void onReplyFinished(QNetworkReply *reply);
	void trySend();
	
protected:
	void onTimer();
	
signals:
	
public slots:
	
private:
	QBasicTimer m_timer;
	QUrl m_base;
	QNetworkReply *m_current;
	ActionList m_actions;
};

} // namespace Control

#endif // CONTROL_NETWORKMANAGER_H

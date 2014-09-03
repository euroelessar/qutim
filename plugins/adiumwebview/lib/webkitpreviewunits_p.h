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

#ifndef WEBKITPREVIEWUNITS_P_H
#define WEBKITPREVIEWUNITS_P_H

#include <qutim/chatunit.h>
#include <qutim/chatsession.h>
#include <qutim/account.h>
#include <qutim/protocol.h>

class WebKitPreviewChatUnit : public qutim_sdk_0_3::ChatUnit
{
	Q_OBJECT
	Q_PROPERTY(QString avatar READ avatar CONSTANT)
public:
	WebKitPreviewChatUnit(const QVariantMap &data, qutim_sdk_0_3::Account *account);
	~WebKitPreviewChatUnit();
	
	QString id() const;
	QString title() const;
	QString avatar() const;
	bool sendMessage(const qutim_sdk_0_3::Message &message);
	
private:
	QVariantMap m_data;
};

class WebKitPreviewAccount : public qutim_sdk_0_3::Account
{
	Q_OBJECT
	Q_PROPERTY(QString avatar READ avatar CONSTANT)
public:
	WebKitPreviewAccount(const QVariantMap &data, qutim_sdk_0_3::Protocol *protocol);
	~WebKitPreviewAccount();

	QString name() const;
	QString avatar() const;
	qutim_sdk_0_3::ChatUnit *getUnit(const QString &unitId, bool create = true);

	void doConnectToServer()
	{
	}

	void doDisconnectFromServer()
	{
	}

	void doStatusChange(const qutim_sdk_0_3::Status &status)
	{
		Q_UNUSED(status);
	}

private:
	QVariantMap m_data;
};

class WebKitPreviewProtocol : public qutim_sdk_0_3::Protocol
{
	Q_OBJECT
	Q_CLASSINFO("Protocol", "preview")
public:
	WebKitPreviewProtocol(QObject *parent);
	~WebKitPreviewProtocol();
	
	QList<qutim_sdk_0_3::Account*> accounts() const;
	qutim_sdk_0_3::Account *account(const QString &id) const;
	void loadAccounts();

private:
};

class WebKitPreviewSession : public qutim_sdk_0_3::ChatSession
{
	Q_OBJECT
public:
	WebKitPreviewSession();
	~WebKitPreviewSession();
	
	qutim_sdk_0_3::ChatUnit *getUnit() const;
	void setChatUnit(qutim_sdk_0_3::ChatUnit* unit);
	QTextDocument *getInputField();
	void markRead(quint64 id);
	qutim_sdk_0_3::MessageList unread() const;
	void addContact(qutim_sdk_0_3::Buddy *c);
	void removeContact(qutim_sdk_0_3::Buddy *c);
	
protected:
	void doSetActive(bool active);
	qint64 doAppendMessage(qutim_sdk_0_3::Message &message);
	
private:
	qutim_sdk_0_3::ChatUnit *m_unit;
};

#endif // WEBKITPREVIEWUNITS_P_H

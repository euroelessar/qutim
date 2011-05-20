/****************************************************************************
**
** qutIM instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@ya.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 2 of the License, or
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

#ifndef SIMPLETRAY_H
#define SIMPLETRAY_H

#include <qutim/messagesession.h>
#include <qutim/account.h>
#include <qutim/protocol.h>
#include <QSystemTrayIcon>
#include <QBasicTimer>
#include <QPixmap>
#include <qutim/icon.h>

namespace Core
{
using namespace qutim_sdk_0_3;

class ProtocolSeparatorActionGenerator;
class StatusAction: public QAction
{
	Q_OBJECT
public:
	StatusAction(QObject* parent);

public slots:
	void onStatusChanged(qutim_sdk_0_3::Status status);
};

class SimpleTray : public MenuController
{
	Q_OBJECT
	Q_CLASSINFO("Service", "TrayIcon")
	Q_CLASSINFO("Uses", "ContactList")
	Q_CLASSINFO("Uses", "ChatLayer")
	Q_CLASSINFO("Uses", "IconLoader")

public:
	SimpleTray();
	~SimpleTray();
	void clActivationStateChanged(bool activated);

private slots:
	void onActivated(QSystemTrayIcon::ActivationReason);
	void onSessionCreated(qutim_sdk_0_3::ChatSession *session);
	void onSessionDestroyed();
	void onUnreadChanged(qutim_sdk_0_3::MessageList unread);
	void onAccountDestroyed(QObject *obj);
	void onAccountCreated(qutim_sdk_0_3::Account *);
	void onStatusChanged(const qutim_sdk_0_3::Status &);

protected:
	virtual void timerEvent(QTimerEvent *);
	QIcon unreadIcon();

private:
	void generateIconSizes(const QIcon &backing, QIcon &icon, int number);

	qint64 activationStateChangedTime;
	void validateProtocolActions();
	QSystemTrayIcon *m_icon;
	QMap<Account*, ActionGenerator*> m_actions;
	QList<ProtocolSeparatorActionGenerator*> m_protocolActions;
	QList<Account*> m_accounts;
	Account *m_activeAccount;
	QList<Protocol*> m_protocols;
	QList<ChatSession*> m_sessions;
	QIcon m_currentIcon;
	QIcon m_generatedIcon;
	QBasicTimer m_iconTimer;
	QIcon m_mailIcon;
	bool m_isMail;
	SettingsItem *m_settingsItem;
};
}

#endif // SIMPLETRAY_H

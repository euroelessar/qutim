/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
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

#ifndef SIMPLETRAY_H
#define SIMPLETRAY_H

#include "simpletraysettings.h"
#include <qutim/chatsession.h>
#include <qutim/account.h>
#include <qutim/protocol.h>
#include <qutim/notification.h>
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

class SimpleTray : public MenuController, public NotificationBackend
{
	Q_OBJECT
	Q_CLASSINFO("Service", "TrayIcon")
	Q_CLASSINFO("Uses", "ContactList")
	Q_CLASSINFO("Uses", "ChatLayer")
	Q_CLASSINFO("Uses", "IconLoader")
	Q_CLASSINFO("Uses", "SettingsLayer")
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
	void onNotificationFinished();
	void reloadSettings();
protected:
	virtual void handleNotification(Notification *notification);
	virtual void timerEvent(QTimerEvent *);
	QIcon unreadIcon();
private:
	QIcon getIconForNotification(Notification *notification);
	void generateIconSizes(const QIcon &backing, QIcon &icon, int number);
	void updateGeneratedIcon();
	void validateProtocolActions();
	Notification *currentNotification();
private:
	qint64 activationStateChangedTime;
	QSystemTrayIcon *m_icon;
	QMap<Account*, ActionGenerator*> m_actions;
	QList<ProtocolSeparatorActionGenerator*> m_protocolActions;
	QList<Account*> m_accounts;
	Account *m_activeAccount;
	QList<Protocol*> m_protocols;
	QHash<ChatSession*, quint64> m_sessions;
	QIcon m_currentIcon;
	QIcon m_generatedIcon;
	QBasicTimer m_iconTimer;
	QIcon m_mailIcon;
	QIcon m_typingIcon;
	QIcon m_chatUserJoinedIcon;
	QIcon m_chatUserLeftIcon;
	QIcon m_qutimIcon;
	QIcon m_transferCompletedIcon;
	QIcon m_birthdayIcon;
	QIcon m_defaultNotificationIcon;
	bool m_showGeneratedIcon;
	SettingsItem *m_settingsItem;

	QList<Notification*> m_messageNotifications;
	QList<Notification*> m_typingNotifications;
	QList<Notification*> m_notifications;
	// Settings
	SimpletraySettings::Option m_showNumber;
	bool m_blink;
	bool m_showIcon;
};
}

#endif // SIMPLETRAY_H


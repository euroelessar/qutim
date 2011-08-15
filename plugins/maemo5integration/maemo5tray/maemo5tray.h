/****************************************************************************
**
** qutIM instant messenger
**
** Copyright (C) 2011 Evgeniy Degtyarev <degtep@gmail.com>
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

#ifndef MAEMO5TRAY_H
#define MAEMO5TRAY_H

#include <qutim/messagesession.h>
#include <qutim/account.h>
#include <qutim/protocol.h>
#include <qutim/notification.h>
#include <QBasicTimer>
#include <QPixmap>
#include <qutim/icon.h>
#include <qutim/iconloader.h>


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

class Maemo5Tray : public MenuController, public NotificationBackend
{
	Q_OBJECT
	Q_CLASSINFO("Service", "TrayIcon")
	Q_CLASSINFO("Uses", "ContactList")
	Q_CLASSINFO("Uses", "ChatLayer")
	Q_CLASSINFO("Uses", "IconLoader")
public:
	Maemo5Tray();
	~Maemo5Tray();
private slots:
	void onActivated();
	void onSessionCreated(qutim_sdk_0_3::ChatSession *session);
	void onSessionDestroyed();
	void onUnreadChanged(qutim_sdk_0_3::MessageList unread);
	void onAccountDestroyed(QObject *obj);
	void onAccountCreated(qutim_sdk_0_3::Account *);
	void onStatusChanged(const qutim_sdk_0_3::Status &);
	void onNotificationFinished();
protected:
	virtual void handleNotification(Notification *notification);
	virtual void timerEvent(QTimerEvent *);

private:
	QString getIconPathForNotification(Notification *notification);
	void updateGeneratedIcon();
	void validateProtocolActions();
	Notification *currentNotification();
private:
	qint64 activationStateChangedTime;
	QMap<Account*, ActionGenerator*> m_actions;
	QList<ProtocolSeparatorActionGenerator*> m_protocolActions;
	QList<Account*> m_accounts;
	Account *m_activeAccount;
	QList<Protocol*> m_protocols;
	QHash<ChatSession*, quint64> m_sessions;
	QString m_offlineIconPath;
	QString m_currentIconPath;
	QString m_generatedIconPath;
	QBasicTimer m_iconTimer;
	QString m_mailIconPath;
	QString m_typingIconPath;
	QString m_chatUserJoinedIconPath;
	QString m_chatUserLeftIconPath;
	QString m_qutimIconPath;
	QString m_transferCompletedIconPath;
	QString m_defaultNotificationIconPath;
	bool m_showGeneratedIcon;


	QList<Notification*> m_messageNotifications;
	QList<Notification*> m_typingNotifications;
	QList<Notification*> m_notifications;

	bool m_blink;
	bool m_showIcon;
};


#endif // MAEMO5TRAY_H

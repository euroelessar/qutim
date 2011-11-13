/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Evgeniy Degtyarev <degtep@gmail.com>
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

#ifndef MAEMO5TRAY_H
#define MAEMO5TRAY_H

#include <qutim/account.h>
#include <qutim/protocol.h>
#include <qutim/notification.h>
#include <QPixmap>
#include <qutim/icon.h>
#include <qutim/iconloader.h>


using namespace qutim_sdk_0_3;


class Maemo5Tray : public QObject, public NotificationBackend
{
	Q_OBJECT
	Q_CLASSINFO("Service", "TrayIcon")
	Q_CLASSINFO("Uses", "ContactList")
	Q_CLASSINFO("Uses", "IconLoader")
public:
	Maemo5Tray();
	~Maemo5Tray();
private slots:
	void onActivated();
	void onAccountCreated(qutim_sdk_0_3::Account *);
	void onStatusChanged(const qutim_sdk_0_3::Status &);
	void onNotificationFinished();
protected:
	virtual void handleNotification(Notification *notification);
private:
	QString getIconPathForNotification(Notification *notification);
	void updateGeneratedIcon();
	Notification *currentNotification();

	QList<Account*> m_accounts;
	Account *m_activeAccount;
	QList<Protocol*> m_protocols;
	QString m_offlineIconPath;
	QString m_currentIconPath;
	QString m_generatedIconPath;
	QString m_mailIconPath;
	QString m_typingIconPath;
	QString m_chatUserJoinedIconPath;
	QString m_chatUserLeftIconPath;
	QString m_qutimIconPath;
	QString m_transferCompletedIconPath;
	QString m_defaultNotificationIconPath;

	QList<Notification*> m_messageNotifications;
	QList<Notification*> m_typingNotifications;
	QList<Notification*> m_notifications;

	bool m_blink;
	bool m_showIcon;
};


#endif // MAEMO5TRAY_H


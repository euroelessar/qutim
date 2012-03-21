/****************************************************************************
**
** qutIM - instant messenger
**
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
#ifndef DBUSNOTIFICATIONSBACKEND_H
#define DBUSNOTIFICATIONSBACKEND_H

#include <qutim/notification.h>
#include <qutim/sound.h>
#include "org_freedesktop_notification.h"

QDBusArgument &operator<< (QDBusArgument &arg, const QImage &image);
const QDBusArgument &operator>> (const QDBusArgument &arg, QImage &image);

class DBusBackend : public QObject, public qutim_sdk_0_3::NotificationBackend
{
	Q_OBJECT
	Q_CLASSINFO("Service", "Popup")
public:
	DBusBackend();
	virtual ~DBusBackend();
	virtual void handleNotification(qutim_sdk_0_3::Notification *notification);
protected slots:
	void callFinished(QDBusPendingCallWatcher* watcher);
	void capabilitiesCallFinished(QDBusPendingCallWatcher* watcher);
	void onActionInvoked(quint32 id, const QString &action_key);
	void onNotificationClosed(quint32 id, quint32 reason);
private:
	struct NotificationData
	{
		QWeakPointer<QObject> sender;
		QString body;
		QList<QWeakPointer<qutim_sdk_0_3::Notification> > notifications;
		QMultiHash<QString, qutim_sdk_0_3::NotificationAction> actions;
	};
	void ignore(NotificationData &data);
private:
	QScopedPointer<org::freedesktop::Notifications> interface;
	QHash<quint32, NotificationData> m_notifications;
	QHash<QObject*, quint32> m_ids;
	QSet<QString> m_capabilities;


};

#endif // DBUSNOTIFICATIONSBACKEND_H


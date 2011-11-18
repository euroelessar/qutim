/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin euroelessar@yandex.ru
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
#include "quicknotificationmanager.h"
#include <qutim/message.h>
#include <qutim/buddy.h>
#include <qutim/debug.h>
#include <qutim/config.h>
#include <qutim/messagesession.h>
#include <qutim/emoticons.h>
#include <QTextDocument>
#include <QVariant>
#include <QBuffer>

namespace MeegoIntegration
{

using namespace qutim_sdk_0_3;

QuickNoficationManager::QuickNoficationManager() :
	NotificationBackend("Popup")
{
	setDescription(QT_TR_NOOP("Show popup"));

}



void QuickNoficationManager::handleNotification(qutim_sdk_0_3::Notification *notification)
{
	ref(notification);
	NotificationRequest request = notification->request();
	NotificationData data;

	QString text = Qt::escape(request.text());



	MNotification *mnotif = new MNotification(MNotification::ImEvent,request.title(),text);
	mnotif->publish();
	if (!request.image().isNull()) {
		//DBusNotifyImageData imageData = { request.image() };
		//hints["image_data"] = qVariantFromValue(imageData);
	}	

	MNotification *id = m_ids.value(request.object());
	if (id) {
		data = m_notifications.value(id);
		data.body = text + "\n" + data.body;
	} else {
		data.sender = request.object();
		data.body = text;
	}

	data.notifications << notification;

	QStringList actions;
	if (m_capabilities.contains(QLatin1String("actions"))) {
		foreach (const NotificationAction &action, request.actions()) {
			QString name = action.title().original();
			data.actions.insert(name, action);
			actions << name << action.title();
		}
	}

	m_notifications.insert(mnotif, data);
	m_ids.insert(request.object(), mnotif);
}


QuickNoficationManager::~QuickNoficationManager()
{
}


void QuickNoficationManager::onActionInvoked(MNotification* id, const QString &name)
{
	NotificationData data = m_notifications.value(id);
	foreach (const NotificationAction &action, data.actions.values(name))
		action.trigger();
}

inline void QuickNoficationManager::ignore(NotificationData &data)
{
	Q_UNUSED(data);
	foreach (const QPointer<Notification> &notification, data.notifications)
		if (notification)
			notification->ignore();
}

void QuickNoficationManager::onNotificationClosed(MNotification* id, quint32 reason)
{
	/*
	  The reasons:
	  1 - The notification expired.
	  2 - The notification was dismissed by the user.
	  3 - The notification was closed by a call to CloseNotification.
	  4 - Undefined/reserved reasons.
	 */

//	QHash<MNotification*, NotificationData>::iterator itr = m_notifications.find(id);
//	if (itr != m_notifications.end()) {
//		if (reason == 2)
//			ignore(*itr);
//		m_ids.remove(itr->sender);
//		foreach (const QPointer<Notification> &notification, itr->notifications)
//			if (notification)
//				deref(notification.data());
//		m_notifications.erase(itr);
//	}
}
}


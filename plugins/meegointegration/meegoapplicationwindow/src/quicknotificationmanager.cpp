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
#include "quicknotificationmanager.h"
#include <qutim/message.h>
#include <qutim/buddy.h>
#include <qutim/debug.h>
#include <qutim/config.h>
#include <qutim/chatsession.h>
#include <qutim/utils.h>
#include <QTextDocument>
#include <QVariant>
#include <QBuffer>
#include "notificationwrapper.h"
#include "addaccountdialogwrapper.h"


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
	if (!m_connected) {
		NotificationWrapper::connect(this);
		m_connected = true;
		qDebug()<<"Connect to qml notificatinManager";

		bool showAc=true;
		foreach (Protocol *proto,Protocol::all()) {
			if (!proto->accounts().isEmpty())
			showAc=false;
		}
		qDebug()<<"Show account creator:"<<showAc;
		if (showAc)
		AddAccountDialogWrapper::showDialog();

	}

	if (!m_active) {
		ref(notification);
		NotificationRequest request = notification->request();


		QString text = Qt::escape(request.text());

		switch (notification->request().type()) {
		case Notification::IncomingMessage:
		case Notification::ChatIncomingMessage: {
			m_notifications << notification;
			QList<MNotification *> notifs= MNotification::notifications();

			for (int i=0; i < notifs.count(); i++)
				notifs.at(i)->remove();

			MNotification *notif = new MNotification(MNotification::ImEvent,
													 tr("qutIM: %n new messages", 0, m_notifications.count()),
													 text);
			notif->publish();
			break;
		}
		default:
			break;
		}
		connect(notification, SIGNAL(finished(qutim_sdk_0_3::Notification::State)),
			SLOT(onNotificationFinished()));
	}
}


QuickNoficationManager::~QuickNoficationManager()
{
}


void QuickNoficationManager::onNotificationFinished()
{
	Notification *notif = sender_cast<Notification*>(sender());
	deref(notif);
	m_notifications.removeOne(notif);
}

void QuickNoficationManager::setWindowActive(bool active)
{
	m_active = active;
}

}


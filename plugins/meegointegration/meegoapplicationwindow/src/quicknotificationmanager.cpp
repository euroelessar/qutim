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
#include <QDBusConnection>


namespace MeegoIntegration
{

using namespace qutim_sdk_0_3;

QuickNotificationManagerAdaptor::QuickNotificationManagerAdaptor(QuickNoficationManager *manager)
	: QDBusAbstractAdaptor(manager), m_manager(manager)
{
}

void QuickNotificationManagerAdaptor::activate()
{
	m_manager->activate();
}

QuickNoficationManager::QuickNoficationManager() :
	NotificationBackend("Popup")
{
	setDescription(QT_TR_NOOP("Show popup"));
	new QuickNotificationManagerAdaptor(this);
	QDBusConnection connection = QDBusConnection::sessionBus();
	connection.registerObject("/NotificationManager", this);
	connection.registerService("org.qutim");
}

QuickNoficationManager::~QuickNoficationManager()
{
	foreach (MNotification *notification, MNotification::notifications())
		notification->remove();
}

void QuickNoficationManager::handleNotification(qutim_sdk_0_3::Notification *notification)
{
	if (!m_connected) {
		NotificationWrapper::connect(this);
		m_connected = true;

		bool showAc=true;
		foreach (Protocol *proto,Protocol::all()) {
			if (!proto->accounts().isEmpty())
			showAc=false;
		}
		if (showAc)
			AddAccountDialogWrapper::showDialog();

	}

	qDebug()<<"Window active state:"<<m_active;
	if (!m_active) {
		ref(notification);
		switch (notification->request().type()) {
		case Notification::IncomingMessage:
		case Notification::ChatIncomingMessage: {
			m_notifications << notification;
			qDebug()<<"Push new notification";
			updateNotification();
			break;
		}
		default:
			break;
		}
		connect(notification, SIGNAL(finished(qutim_sdk_0_3::Notification::State)),
			SLOT(onNotificationFinished()));
	}
}

void QuickNoficationManager::setWindowActive(bool active)
{
	m_active = active;
}

void QuickNoficationManager::updateNotification()
{
	QList<MNotification *> notifs = MNotification::notifications();

	for (int i = 0; i < notifs.count(); i++)
		notifs.at(i)->remove();

	if (m_notifications.isEmpty())
		return;

	MNotification *notification = new MNotification(MNotification::ImReceivedEvent);
	MRemoteAction action("org.qutim",
						 "/NotificationManager",
						 "org.qutim.NoficationManager",
						 "activate");
	NotificationRequest request = m_notifications.last()->request();
	QString text;
	if (request.object()) {
		text = request.object()->property("title").toString();
		text += QLatin1String(": ");
	}
	text += Qt::escape(request.text());
	notification->setBody(text);
	notification->setSummary(tr("qutIM: %n new messages", 0, m_notifications.count()));
	notification->setAction(action);
	notification->publish();
}

void QuickNoficationManager::activate()
{
	QSet<ChatUnit*> objects;
	foreach (Notification *notification, m_notifications)
		objects << qobject_cast<ChatUnit*>(notification->request().object());
	objects.remove(0);
	if (objects.size() == 1) {
		ChatSession *session = ChatLayer::get(*objects.begin(), false);
		if (session) {
			emit requestChannel(session);
			return;
		}
	}
	emit requestChannelList();
}

void QuickNoficationManager::onNotificationFinished()
{
	Notification *notif = sender_cast<Notification*>(sender());
	deref(notif);
	m_notifications.removeOne(notif);
	updateNotification();
}

}


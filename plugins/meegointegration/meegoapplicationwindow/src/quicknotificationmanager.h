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
#ifndef QUICKNOTIFICATIONMANAGER_H
#define QUICKNOTIFICATIONMANAGER_H

#include <qutim/notification.h>
#include <qutim/sound.h>
#include <QSet>
#include <MNotification>

namespace MeegoIntegration
{

using namespace qutim_sdk_0_3;

class QuickNoficationManager : public QObject, public qutim_sdk_0_3::NotificationBackend
{
	Q_OBJECT
	Q_CLASSINFO("Service", "Popup")
public:
	QuickNoficationManager();
	virtual ~QuickNoficationManager();
	virtual void handleNotification(qutim_sdk_0_3::Notification *notification);
	void setWindowActive(bool active);
	
protected slots:
	void onNotificationFinished();
	
private:
	QList<Notification*> m_notifications;
	QHash<QObject*, MNotification*> m_ids;
	QSet<QString> m_capabilities;
	bool m_active;
	bool m_connected;


};
}

#endif // QUICKNOTIFICATIONMANAGER_H


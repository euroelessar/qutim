/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Nicolay Izoderov <nico-izo@ya.ru>
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
#include "haikunotificationsbackend.h"
#include <qutim/message.h>
#include <qutim/buddy.h>
#include <qutim/debug.h>
#include <qutim/config.h>
#include <qutim/chatsession.h>
#include <QTextDocument>

#ifdef Q_OS_HAIKU
#include <Roster.h>
#include <Notification.h>
#include <String.h>
#include <Bitmap.h>
#endif

using namespace qutim_sdk_0_3;

HaikuNotificationsBackend::HaikuNotificationsBackend() :
	NotificationBackend("Popup")
{
	setDescription(QT_TR_NOOP("Show popup"));
}



void HaikuNotificationsBackend::handleNotification(qutim_sdk_0_3::Notification *notification)
{
	//ref(notification);
	NotificationRequest request = notification->request();
	NotificationData data;

	QString text = request.text();

	BNotification notify(B_INFORMATION_NOTIFICATION);
	notify.SetTitle(static_cast<BString>(request.title().toLocal8Bit()));
	notify.SetContent(static_cast<BString>(text.toLocal8Bit()));
	if(!request.image().isNull()) {
		BBitmap *bitmap = request.image().toHaikuBitmap();
		notify.setIcon(bitmap);
	}
		
	notify.Send();
}


HaikuNotificationsBackend::~HaikuNotificationsBackend()
{
}

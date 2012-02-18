/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#ifndef NOTIFICATIONSSETTINGS_H
#define NOTIFICATIONSSETTINGS_H

#include <QObject>
#include <qutim/startupmodule.h>
#include <qutim/notification.h>
#include "notificationsettings.h"

namespace qutim_sdk_0_3 {
class SettingsItem;
}

namespace Core
{

class NotifyEnabler;
class NotificationsSettings : public QObject
{
	Q_OBJECT
	Q_CLASSINFO("Service", "NotificationSettings")
public:
	explicit NotificationsSettings(QObject *parent = 0);
	~NotificationsSettings();
private:
	qutim_sdk_0_3::SettingsItem *m_settings;
	NotifyEnabler *m_enabler;
};

class NotifyEnabler: public QObject, public qutim_sdk_0_3::NotificationFilter
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::NotificationFilter)
public:
	NotifyEnabler(QObject *parent = 0);
public slots:
	void enabledTypesChanged(const EnabledNotificationTypes &enabledTypes);
	void reloadSettings();
	void onBackendCreated(const QByteArray &type);
	void onBackendDestroyed(const QByteArray &type);
protected:
	virtual void filter(qutim_sdk_0_3::NotificationRequest &request);
private:
	EnabledNotificationTypes m_enabledTypes;
	bool m_notificationsInActiveChat;
	bool m_ignoreConfMsgsWithoutUserNick;
};

}

#endif // NOTIFICATIONSSETTINGS_H


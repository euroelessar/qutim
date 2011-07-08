/****************************************************************************
 *  mobilenotificationssettings.h
 *
 *  Copyright (c) 2011 by Sidorov Aleksey <sauron@citadelspb.com>
 *                        Prokhin Alexey <alexey.prokhin@yandex.ru>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#ifndef MOBILENOTIFICATIONSSETTINGS_H
#define MOBILENOTIFICATIONSSETTINGS_H

#include <QObject>
#include <qutim/startupmodule.h>
#include <qutim/notification.h>
#include "mobilenotificationsettings.h"

namespace qutim_sdk_0_3 {
class SettingsItem;
}

namespace Core
{

typedef QList<QSet<QByteArray> > EnabledNotificationTypes;
class MobileNotifyEnabler;

class MobileNotificationsSettings : public QObject
{
	Q_OBJECT
	Q_CLASSINFO("Service", "NotificationSettings")
public:
	explicit MobileNotificationsSettings(QObject *parent = 0);
	~MobileNotificationsSettings();
private:
	qutim_sdk_0_3::SettingsItem *m_settings;
	MobileNotifyEnabler *m_enabler;
};

class MobileNotifyEnabler: public QObject, public qutim_sdk_0_3::NotificationFilter
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::NotificationFilter)
public:
	MobileNotifyEnabler(QObject *parent = 0);
public slots:
	void reloadSettings();
	void onBackendCreated(const QByteArray &type);
	void onBackendDestroyed(const QByteArray &type);
protected:
	virtual void filter(qutim_sdk_0_3::NotificationRequest& request);
private:
	EnabledNotificationTypes m_enabledTypes;
	bool m_notificationsInActiveChat;
	bool m_ignoreConfMsgsWithoutUserNick;
};

}

#endif // MOBILENOTIFICATIONSSETTINGS_H

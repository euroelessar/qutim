/****************************************************************************
 *  notificationssettings.h
 *
 *  Copyright (c) 2011 by Sidorov Aleksey <sauron@citadelspb.com>
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

#ifndef NOTIFICATIONSSETTINGS_H
#define NOTIFICATIONSSETTINGS_H

#include <QObject>
#include <qutim/startupmodule.h>
#include <qutim/notification.h>

namespace qutim_sdk_0_3 {
class SettingsItem;
}

namespace Core
{

class NotifyEnabler;
class NotificationsSettings : public QObject, public qutim_sdk_0_3::StartupModule
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::StartupModule)
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
	void loadSettings();
protected:
    virtual Result filter(qutim_sdk_0_3::NotificationRequest& request);
private:
	int m_flags;
};

}

#endif // NOTIFICATIONSSETTINGS_H

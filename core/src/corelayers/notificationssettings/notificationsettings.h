/****************************************************************************
 *  notificationsettings.h
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

#ifndef CORE_NOTIFICATIONSETTINGS_H
#define CORE_NOTIFICATIONSETTINGS_H
#include <qutim/settingswidget.h>
#include <qutim/notification.h>

class QCheckBox;
namespace Core {

class NotificationSettings : public qutim_sdk_0_3::SettingsWidget
{
    Q_OBJECT
public:
    explicit NotificationSettings(QWidget *parent = 0);
    virtual void loadImpl();
    virtual void cancelImpl();
    virtual void saveImpl();
protected:
	void addNotify(qutim_sdk_0_3::Notification::Type type, bool set);
private:
	QMap<qutim_sdk_0_3::Notification::Type, QCheckBox*> m_boxMap;
};

} // namespace Core

#endif // CORE_NOTIFICATIONSETTINGS_H

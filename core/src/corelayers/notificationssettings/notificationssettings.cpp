/****************************************************************************
 *  notificationssettings.cpp
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

#include "notificationssettings.h"
#include <qutim/settingslayer.h>
#include <qutim/icon.h>
#include <qutim/config.h>

namespace Core {

using namespace qutim_sdk_0_3;

NotificationsSettings::NotificationsSettings(QObject *parent) :
	QObject(parent),
	m_enabler(new NotifyEnabler(this))
{
	m_settings = new GeneralSettingsItem<Core::NotificationSettings>(
						Settings::General,
						Icon("dialog-information"),
						QT_TRANSLATE_NOOP("Settings","Notifications"));
	Settings::registerItem(m_settings);
	m_settings->connect(SIGNAL(enabledTypesChanged(EnabledNotificationTypes)),
						m_enabler,
						SLOT(enabledTypesChanged(EnabledNotificationTypes)));

	NotificationFilter::registerFilter(m_enabler, NotificationFilter::HighPriority);
}

NotificationsSettings::~NotificationsSettings()
{
	NotificationFilter::unregisterFilter(m_enabler);
	Settings::removeItem(m_settings);
	delete m_settings;
}

NotifyEnabler::NotifyEnabler(QObject* parent): QObject(parent)
{
	m_enabledTypes = NotificationSettings::enabledTypes();
}

void NotifyEnabler::enabledTypesChanged(const EnabledNotificationTypes &enabledTypes)
{
	m_enabledTypes = enabledTypes;
}

NotificationFilter::Result NotifyEnabler::filter(NotificationRequest &request)
{
	Notification::Type type = request.type();
	if (type >= 0 && type < m_enabledTypes.size())
		request.setBackends(m_enabledTypes.at(type));

	return NotificationFilter::Accept;
}

}

/****************************************************************************
 *  notificationssettings.cpp
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

#include "notificationssettings.h"
#include "notificationsettings.h"
#include <qutim/settingslayer.h>
#include <qutim/icon.h>
#include <qutim/config.h>

namespace Core {

using namespace qutim_sdk_0_3;

NotifyEnabler::NotifyEnabler(QObject* parent): QObject(parent)
{
	loadSettings();
}

void NotifyEnabler::loadSettings()
{
	Config cfg;
	cfg.beginGroup("notification");
	m_flags = cfg.value("flags", -1);
}

NotificationFilter::Result NotifyEnabler::filter(NotificationRequest& request)
{
	if (request.type() & m_flags)
		return NotificationFilter::Accept;
	else
		return NotificationFilter::Reject;
}


NotificationsSettings::NotificationsSettings(QObject *parent) :
	QObject(parent),
	m_enabler(new NotifyEnabler(this))
{
	m_settings = new GeneralSettingsItem<Core::NotificationSettings>(Settings::General,Icon("dialog-information"),QT_TRANSLATE_NOOP("Settings","Notifications"));
	Settings::registerItem(m_settings);
	NotificationFilter::registerFilter(m_enabler, NotificationFilter::HighPriority);

	m_settings->connect(SIGNAL(saved()), m_enabler, SLOT(loadSettings()));
}

NotificationsSettings::~NotificationsSettings()
{
	NotificationFilter::unregisterFilter(m_enabler);
	Settings::removeItem(m_settings);
	delete m_settings;
}

}

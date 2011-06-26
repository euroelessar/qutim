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
#include <qutim/chatunit.h>
#include <qutim/chatsession.h>

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

	reloadSettings();
}

void NotifyEnabler::reloadSettings()
{
	Config cfg = Config("appearance").group("chat");
	m_notificationsInActiveChat = cfg.value("notificationsInActiveChat", true);
}

NotificationFilter::Result NotifyEnabler::filter(NotificationRequest &request)
{
	Notification::Type type = request.type();

	// Block notification from an active session, if the notifications are disabled by user.
	// TODO: maybe we should not block notifications about outgoing messages, they are almost
	// always from an active session, so essentially the option disables all notification
	// about the outgoing messages which already could be done via notification types settings.
	// Do we want two different options that basically do the same thing?
	// By the way, when (and if) you will do it, don't forget to do the same in mobile notification
	// settings.
	if (!m_notificationsInActiveChat) {
		if (ChatUnit *unit = qobject_cast<ChatUnit*>(request.object())) {
			if (ChatSession *session = ChatLayer::get(unit, false)) {
				if (session->isActive())
					return NotificationFilter::Reject;
			}
		}
	}

	if (type >= 0 && type < m_enabledTypes.size())
		request.setBackends(m_enabledTypes.at(type));

	return NotificationFilter::Accept;
}

}

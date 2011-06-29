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
#include <qutim/conference.h>
#include <qutim/event.h>

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
	Event::eventManager()->installEventFilter(this);
}

void NotifyEnabler::enabledTypesChanged(const EnabledNotificationTypes &enabledTypes)
{
	m_enabledTypes = enabledTypes;

	reloadSettings();
}

void NotifyEnabler::reloadSettings()
{
	Config cfg = Config().group("notification");
	m_ignoreConfMsgsWithoutUserNick = cfg.value("ignoreConfMsgsWithoutUserNick", true);

	cfg = Config("appearance").group("chat");
	m_notificationsInActiveChat = cfg.value("notificationsInActiveChat", true);
}

void NotifyEnabler::filter(NotificationRequest &request)
{
	Notification::Type type = request.type();

	// Reject notification from an active session, if the notifications are disabled by user.
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
					request.reject("sessionIsActive");
			}
		}
	}

	if (m_ignoreConfMsgsWithoutUserNick &&
		type == Notification::IncomingMessage ||
		type == Notification::OutgoingMessage ||
		type == Notification::ChatIncomingMessage ||
		type == Notification::ChatOutgoingMessage)
	{
		// Reject conference messages that do not contain user nick
		if (Conference *conf = qobject_cast<Conference*>(request.object())) {
			QString msg = request.text();
			Buddy *me = conf->me();
			if (me && !msg.contains(me->name()) && !msg.contains(me->id()))
				request.reject("confMessageWithoutUserNick");
		}
	}

	if (type >= 0 && type < m_enabledTypes.size())
		request.setBackends(m_enabledTypes.at(type));
}

bool NotifyEnabler::eventFilter(QObject *obj, QEvent *ev)
{
	if (ev->type() == Event::eventType()) {
		Event *event = static_cast<Event*>(ev);
		static quint16 backendRegistered = Event::registerType("notification-backend-registered");
		static quint16 backendRemoved = Event::registerType("notification-backend-removed");

		if (event->id == backendRegistered) {
			Config cfg;
			cfg.beginGroup("notification");
			QByteArray backendType = event->args[0].toByteArray();
			for (int i = 0; i <= Notification::LastType; ++i) {
				cfg.beginGroup(notificationTypeName(static_cast<Notification::Type>(i)));
				if (cfg.value(backendType, true))
					m_enabledTypes[i] << backendType;
				cfg.endGroup();
			}
			cfg.endGroup();
			event->accept();
			return true;
		} else if (event->id == backendRemoved) {
			QByteArray backendType = event->args[0].toByteArray();
			// Before removing the backend settings, check that another backend
			// does not have the same type.
			if (!NotificationBackend::allTypes().contains(backendType)) {
				for (int i = 0; i <= Notification::LastType; ++i)
					m_enabledTypes[i].remove(backendType);
			}
			event->accept();
			return true;
		}
	}
	return QObject::eventFilter(obj, ev);
}

}

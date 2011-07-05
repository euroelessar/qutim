/****************************************************************************
 *  mobilenotificationssettings.cpp
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

#include "mobilenotificationssettings.h"
#include <qutim/settingslayer.h>
#include <qutim/icon.h>
#include <qutim/config.h>
#include <qutim/chatsession.h>
#include <qutim/chatunit.h>
#include <qutim/conference.h>
#include <qutim/event.h>
#include <QTimer>

namespace Core {

using namespace qutim_sdk_0_3;

MobileNotificationsSettings::MobileNotificationsSettings(QObject *parent) :
	QObject(parent),
	m_enabler(new MobileNotifyEnabler(this))
{
	m_settings = new GeneralSettingsItem<Core::MobileNotificationSettings>(
						Settings::General,
						Icon("dialog-information"),
						QT_TRANSLATE_NOOP("Settings","Notifications"));
	Settings::registerItem(m_settings);
	m_settings->connect(SIGNAL(saved()), m_enabler, SLOT(reloadSettings()));

	NotificationFilter::registerFilter(m_enabler, NotificationFilter::HighPriority);
}

MobileNotificationsSettings::~MobileNotificationsSettings()
{
	NotificationFilter::unregisterFilter(m_enabler);
	Settings::removeItem(m_settings);
	delete m_settings;
}

MobileNotifyEnabler::MobileNotifyEnabler(QObject* parent): QObject(parent)
{
	reloadSettings();
	connect(NotificationManager::instance(),
			SIGNAL(backendCreated(QByteArray,NotificationBackend*)),
			SLOT(onBackendCreated(QByteArray)));
	connect(NotificationManager::instance(),
			SIGNAL(backendDestroyed(QByteArray,NotificationBackend*)),
			SLOT(onBackendDestroyed(QByteArray)));
}

void MobileNotifyEnabler::reloadSettings()
{
	m_enabledTypes.clear();
	Config cfg;
	cfg.beginGroup("notification");

	for (int i = 0; i <= Notification::LastType; ++i) {
		Notification::Type type = static_cast<Notification::Type>(i);
		QSet<QByteArray> backendTypes;
		cfg.beginGroup(notificationTypeName(type));
		foreach (NotificationBackend *backend, NotificationBackend::all()) {
			QByteArray backendType = backend->backendType();
			if (cfg.value(backendType, true))
				backendTypes << backendType;
		}
		cfg.endGroup();
		m_enabledTypes << backendTypes;
	}

	m_ignoreConfMsgsWithoutUserNick = cfg.value("ignoreConfMsgsWithoutUserNick", true);
	cfg.endGroup();

	cfg = Config("appearance").group("chat");
	m_notificationsInActiveChat = cfg.value("notificationsInActiveChat", true);
}

void MobileNotifyEnabler::onBackendCreated(const QByteArray &type)
{
	Config cfg;
	cfg.beginGroup("notification");
	for (int i = 0; i <= Notification::LastType; ++i) {
		cfg.beginGroup(notificationTypeName(static_cast<Notification::Type>(i)));
		if (cfg.value(type, true))
			m_enabledTypes[i] << type;
		cfg.endGroup();
	}
	cfg.endGroup();
}

void MobileNotifyEnabler::onBackendDestroyed(const QByteArray &type)
{
	// Before removing the backend settings, check that another backend
	// does not have the same type.
	if (!NotificationBackend::allTypes().contains(type)) {
		for (int i = 0; i <= Notification::LastType; ++i)
			m_enabledTypes[i].remove(type);
	}
}

void MobileNotifyEnabler::filter(NotificationRequest &request)
{
	Notification::Type type = request.type();

	// Block notification from an active session, if the notifications are disabled by user.
	if (!m_notificationsInActiveChat) {
		if (ChatUnit *unit = qobject_cast<ChatUnit*>(request.object())) {
			if (ChatSession *session = ChatLayer::get(unit, false)) {
				if (session->isActive())
					request.reject("sessionIsActive");
			}
		}
	}

	if (m_ignoreConfMsgsWithoutUserNick &&
		(type == Notification::IncomingMessage ||
		type == Notification::OutgoingMessage ||
		type == Notification::ChatIncomingMessage ||
		type == Notification::ChatOutgoingMessage))
	{
		// Ignore conference messages that do not contain user nick
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

}

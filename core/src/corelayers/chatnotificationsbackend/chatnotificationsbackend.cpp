/****************************************************************************
 *  chatnotificationsbackend.cpp
 *
 *  Copyright (c) 2011 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#include "chatnotificationsbackend.h"
#include <QDateTime>

namespace Core {

using namespace qutim_sdk_0_3;

ChatNotificationsBackend::ChatNotificationsBackend() :
	NotificationBackend("ChatNotifications")
{
	connect(ChatLayer::instance(), SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)),
			SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*)));
}

void ChatNotificationsBackend::handleNotification(Notification *notification)
{
	NotificationRequest request = notification->request();

	Notification::Type type = request.type();
	if (type == Notification::IncomingMessage ||
		type == Notification::OutgoingMessage ||
		type == Notification::ChatIncomingMessage ||
		type == Notification::ChatOutgoingMessage)
	{
		return;
	}

	ChatSession *session = 0;
	if (ChatUnit *unit = qobject_cast<ChatUnit*>(request.object())) {
		session = ChatLayer::get(unit);
		Q_ASSERT(session);
	} else {
		session = m_currentSession.data();
		if (!session)
			return;
	}

	QString text = request.text();
	if (text.isEmpty())
		text = request.title();

	Message msg(text);
	msg.setTime(QDateTime::currentDateTime());
	msg.setIncoming(true);
	msg.setChatUnit(session->unit());
	msg.setProperty("silent", true);
	msg.setProperty("service", true);
	msg.setProperty("store", false);
	session->appendMessage(msg);
}

void ChatNotificationsBackend::onSessionCreated(ChatSession *session)
{
	connect(session, SIGNAL(activated(bool)), SLOT(onSessionActivated(bool)));
}

void ChatNotificationsBackend::onSessionActivated(bool active)
{
	if (active)
		m_currentSession = qobject_cast<ChatSession*>(sender());
}

} // namespace Core

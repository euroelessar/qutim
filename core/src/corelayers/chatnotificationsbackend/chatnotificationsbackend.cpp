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
#include <qutim/account.h>
#include <QDateTime>

namespace Core {

using namespace qutim_sdk_0_3;

ChatNotificationsBackend::ChatNotificationsBackend() :
	NotificationBackend("ChatNotifications")
{
	setDescription(QT_TR_NOOP("Add message to chat"));
	allowRejectedNotifications("sessionIsActive");
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

	QString text = request.text();
	if (text.isEmpty())
		text = request.title();
	Message msg(text);
	msg.setTime(QDateTime::currentDateTime());
	msg.setIncoming(true);
	msg.setProperty("silent", true);
	msg.setProperty("service", true);
	msg.setProperty("store", false);

	ChatSession *session = 0;
	if (ChatUnit *unit = qobject_cast<ChatUnit*>(request.object())) {
		msg.setChatUnit(unit);
		session = ChatLayer::get(unit, false);
		if (!session) {
			ChatUnit *sessionUnit = unit->account()->getUnitForSession(unit);
			MessageList &messages = m_unaddedMessages[sessionUnit];
			if (messages.isEmpty())
				connect(sessionUnit, SIGNAL(destroyed()), SLOT(onUnitDestroyed()));
			messages.push_back(msg);
		} else {
			session->appendMessage(msg);
		}
	} else {
		session = m_currentSession.data();
		if (session) {
			msg.setChatUnit(session->unit());
			session->appendMessage(msg);
		}
	}
}

void ChatNotificationsBackend::onSessionCreated(ChatSession *session)
{
	connect(session, SIGNAL(activated(bool)), SLOT(onSessionActivated(bool)));

	MessageList messages = m_unaddedMessages.take(session->unit());
	foreach (Message message, messages)
		session->appendMessage(message);
	if (!messages.isEmpty())
		disconnect(session->unit(), SIGNAL(destroyed()), this, SLOT(onUnitDestroyed()));
}

void ChatNotificationsBackend::onSessionActivated(bool active)
{
	if (active)
		m_currentSession = qobject_cast<ChatSession*>(sender());
}

void ChatNotificationsBackend::onUnitDestroyed()
{
	ChatUnit *unit = static_cast<ChatUnit*>(sender());
	m_unaddedMessages.remove(unit);
}

} // namespace Core

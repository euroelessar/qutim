/****************************************************************************
 *  notificationfilter.cpp
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

#include "notificationfilter.h"
#include <qutim/metacontact.h>
#include <qutim/event.h>
#include <qutim/conference.h>
#include <qutim/utils.h>
#include <QTimer>
#include <QCoreApplication>

namespace Core {

using namespace qutim_sdk_0_3;

static QString toString(Notification::Type type)
{
	QString title;
	switch(type) {
	case Notification::IncomingMessage:
	case Notification::ChatIncomingMessage:
		title = QObject::tr("Message from %1:");
		break;
	case Notification::OutgoingMessage:
	case Notification::ChatOutgoingMessage:
		title = QObject::tr("Message to %1:");
		break;
	case Notification::AppStartup:
		title = QObject::tr("qutIM launched");
		break;
	case Notification::BlockedMessage:
		title = QObject::tr("Blocked message from %1");
		break;
	case Notification::ChatUserJoined:
		title = QObject::tr("%1 has joined");
		break;
	case Notification::UserOnline:
		title = QObject::tr("%1 is online");
		break;
	case Notification::ChatUserLeft:
		title = QObject::tr("%1 has left");
		break;
	case Notification::UserOffline:
		title = QObject::tr("%1 is offline");
		break;
	case Notification::UserChangedStatus:
		title = QObject::tr("%1 changed status");
		break;
	case Notification::UserHasBirthday:
		title = QObject::tr("%1 has birthday today!!");
		break;
	case Notification::UserTyping:
		title = QObject::tr("%1 is typing");
		break;
	case Notification::FileTransferCompleted:
	case Notification::System:
	default:
		title = QObject::tr("System notify");
	}
	return title;
}

// TODO: maybe we should move it to libqutim?
static inline ChatUnit *getRealUnit(QObject *obj)
{
	ChatUnit *unit = qobject_cast<ChatUnit*>(obj);
	if (!unit)
		return 0;

	static quint16 realUnitRequestEvent = Event::registerType("real-chatunit-request");
	Event event(realUnitRequestEvent);
	QCoreApplication::sendEvent(unit, &event);

	if (qobject_cast<Conference*>(unit))
		return unit;

	Contact *contact = event.at<Contact*>(0);
	while (unit && !contact) {
		if (!!(contact = qobject_cast<Contact*>(unit)))
			break;
		unit = unit->upperUnit();
	}

	if (Contact *meta = qobject_cast<MetaContact*>(contact->metaContact()))
		contact = meta;

	return contact;
}

NotificationFilterImpl::NotificationFilterImpl()
{
	registerFilter(this, LowPriority);
	connect(ChatLayer::instance(), SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)),
			SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*)));
}

NotificationFilterImpl::~NotificationFilterImpl()
{
	unregisterFilter(this);
}

void NotificationFilterImpl::filter(NotificationRequest &request)
{
	QString sender_name = request.property("senderName", QString());
	QObject *sender = request.object();
	if (!sender) {
		request.setTitle(toString(request.type()).arg(sender_name));
		return;
	}

	if (request.title().isEmpty()) {
		if (sender) {
			sender_name = sender->property("title").toString();
			if (sender_name.isEmpty())
				sender_name = sender->property("name").toString();
			if(sender_name.isEmpty())
				sender_name = sender->property("id").toString();
		}
		QString title = toString(request.type()).arg(sender_name);
		request.setTitle(title);
	}

	if (request.image().isNull()) {
		QString avatar = sender->property("avatar").toString();
		request.setImage(QPixmap(avatar));
	}


	switch (request.type()) {
	case Notification::OutgoingMessage:
	case Notification::IncomingMessage:
	case Notification::ChatIncomingMessage:
	case Notification::ChatOutgoingMessage:
	case Notification::UserTyping:
	case Notification::UserChangedStatus:
	case Notification::BlockedMessage:
	case Notification::ChatUserJoined:
	case Notification::ChatUserLeft:
		{
			NotificationAction action(QObject::tr("Open chat"),
									  this,
									  SLOT(onOpenChatClicked(qutim_sdk_0_3::NotificationRequest)));
			action.setType(NotificationAction::AcceptButton);
			request.addAction(action);
		}
		{
			NotificationAction action(QObject::tr("Ignore"),
									  this,
									  SLOT(onIgnoreChatClicked(qutim_sdk_0_3::NotificationRequest)));
			action.setType(NotificationAction::IgnoreButton);
			request.addAction(action);
		}
		break;
	default:
		break;
	}
}

void NotificationFilterImpl::notificationCreated(Notification *notification)
{
	NotificationRequest request = notification->request();

	ChatUnit *unit = getRealUnit(request.object());
	if (unit) {
		// If the chatunit's session is not active, show the notification until
		// it is activated; otherwise, show the notification only for a few
		// seconds.
		ChatSession *session = ChatLayer::get(unit);
		if (session->isActive()) {
			QTimer::singleShot(5000, notification, SLOT(deleteLater()));
		} else {
			m_notifications.insert(unit, notification);
			connect(notification, SIGNAL(destroyed()), SLOT(onNotificationDestroyed()));
		}
	}
}

void NotificationFilterImpl::onOpenChatClicked(const NotificationRequest &request)
{
	ChatUnit *unit = qobject_cast<ChatUnit*>(request.object());
	if (!unit)
		return;
	ChatSession *session = ChatLayer::get(unit);
	if (session)
		session->activate();
}

void NotificationFilterImpl::onIgnoreChatClicked(const NotificationRequest &request)
{
	QVariant msgVar = request.property("message", QVariant());
	if (msgVar.isNull())
		return;
	ChatUnit *unit = qobject_cast<ChatUnit*>(request.object());
	if (!unit)
		return;
	ChatSession *session = ChatLayer::get(unit, false);
	if (session)
		session->markRead(msgVar.value<Message>().id());
}

void NotificationFilterImpl::onSessionCreated(qutim_sdk_0_3::ChatSession *session)
{
	connect(session, SIGNAL(activated(bool)), SLOT(onSessionActivated(bool)));
}

void NotificationFilterImpl::onSessionActivated(bool active)
{
	if (!active)
		return;
	ChatSession *session = sender_cast<ChatSession*>(sender());
	ChatUnit *unit = getRealUnit(session->unit());
	if (unit) {
		foreach (Notification *notification, m_notifications.values(unit))
			notification->deleteLater();
		m_notifications.remove(unit);
	}
}

void NotificationFilterImpl::onNotificationDestroyed()
{
	Notification *notification = static_cast<Notification*>(sender());
	Notifications::iterator itr = m_notifications.begin();
	Notifications::iterator end = m_notifications.end();
	for (; itr != end; ++itr) {
		if (*itr == notification) {
			m_notifications.erase(itr);
			return;
		}
	}
}

} // namespace Core

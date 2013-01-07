/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#include "notificationfilter.h"
#include <qutim/metacontact.h>
#include <qutim/event.h>
#include <qutim/conference.h>
#include <qutim/utils.h>
#include <qutim/account.h>
#include <qutim/protocol.h>
#include <QTimer>
#include <QCoreApplication>

namespace Core {

using namespace qutim_sdk_0_3;

static QString toString(Notification::Type type, const QString &argument)
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
		// We don't have place for argument
		return title;
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
		title = QObject::tr("%1 has changed status to %2");
		break;
	case Notification::UserHasBirthday:
		title = QObject::tr("%1 has birthday today!!");
		break;
	case Notification::UserTyping:
		title = QObject::tr("%1 is typing");
		break;
	case Notification::Attention:
		title = QObject::tr("%1 is asking for your attention");
		break;
	case Notification::FileTransferCompleted:
	case Notification::System:
	default:
		title = QObject::tr("System notify");
		// We don't have place for argument
		return title;
	}
	return title.arg(argument);
}

static inline ChatUnit *getUnitForSession(QObject *obj)
{
	ChatUnit *unit = qobject_cast<ChatUnit*>(obj);
	if (ChatUnit *contact = unit->metaContact())
			unit = contact;
	return unit ? unit->account()->getUnitForSession(unit) : 0;
}

NotificationFilterImpl::NotificationFilterImpl()
{
	registerFilter(this, LowPriority);
	connect(ChatLayer::instance(), SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)),
			SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*)));

	foreach (Protocol *proto, Protocol::all()) {
		foreach (Account *acc, proto->accounts())
			onAccountCreated(acc);
		connect(proto, SIGNAL(accountCreated(qutim_sdk_0_3::Account*)),
				SLOT(onAccountCreated(qutim_sdk_0_3::Account*)));
	}
}

NotificationFilterImpl::~NotificationFilterImpl()
{
	unregisterFilter(this);
}

void NotificationFilterImpl::filter(NotificationRequest &request)
{
	Notification::Type reqType = request.type();
	QString sender_name = request.property("senderName", QString());
	QObject *sender = request.object();
	if (!sender) {
		request.setTitle(toString(reqType, sender_name));
		return;
	}

	if (request.title().isEmpty()) {
		if (sender && sender_name.isEmpty()) {
			sender_name = sender->property("title").toString();
			if (sender_name.isEmpty())
				sender_name = sender->property("name").toString();
			if(sender_name.isEmpty())
				sender_name = sender->property("id").toString();
		}
		QString title = toString(request.type(), sender_name);
		if (reqType == Notification::UserChangedStatus) {
			Status status = request.property("status", Status());
			title = title.arg(status.name().toString());
		}
		request.setTitle(title);
	}

	if (request.image().isNull()) {
		QString avatar = sender->property("avatar").toString();
		request.setImage(QPixmap(avatar));
	}


	switch (reqType) {
	case Notification::UserChangedStatus:
	case Notification::UserOnline:
	case Notification::UserOffline: {
		Buddy *buddy = qobject_cast<Buddy*>(request.object());
		if (buddy) {
			Account *acc = buddy->account();
			Status::Type status = acc->status().type();
			if (status == Status::Offline || status == Status::Connecting ||
				m_connectingAccounts.contains(buddy->account()))
			{
				// We don't want the notification because the buddy's account is
				// either loading its roster or offline.
				request.reject("loadingRoster");
			}
		}

		// fall through
	}
	case Notification::OutgoingMessage:
	case Notification::IncomingMessage:
	case Notification::ChatIncomingMessage:
	case Notification::ChatOutgoingMessage:
	case Notification::UserTyping:
	case Notification::BlockedMessage:
	case Notification::ChatUserJoined:
	case Notification::ChatUserLeft:
	case Notification::UserHasBirthday:
	case Notification::Attention:
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
	Notification::Type type = request.type();

	if (type == Notification::UserChangedStatus ||
		type == Notification::UserOnline ||
		type == Notification::UserOffline)
	{
		// I am not sure how to handle the notification,
		// so I am just trying to imitate miranda's behaviour.
		QTimer::singleShot(5000, notification, SLOT(reject()));
		return;
	}

	ChatUnit *unit = getUnitForSession(request.object());
	if (unit) {
		// If the chatunit's session is not active, show the notification until
		// it is activated; otherwise, show the notification only for a few
		// seconds.
		ChatSession *session = ChatLayer::get(unit);
		if (session->isActive()) {
			QTimer::singleShot(5000, notification, SLOT(reject()));
		} else {
			m_notifications.insert(unit, notification);
			connect(notification, SIGNAL(finished(qutim_sdk_0_3::Notification::State)),
					SLOT(onNotificationFinished()));
			connect(unit, SIGNAL(destroyed()), SLOT(onUnitDestroyed()), Qt::UniqueConnection);
		}
	}
}

void NotificationFilterImpl::onOpenChatClicked(const NotificationRequest &request)
{
	ChatUnit *unit = getUnitForSession(request.object());
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
	ChatUnit *unit = getUnitForSession(request.object());
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
	ChatUnit *unit = getUnitForSession(session->unit());
	if (unit) {
		foreach (const QPointer<Notification> &notification, m_notifications.values(unit)) {
			if (notification)
				notification.data()->reject();
		}
		m_notifications.remove(unit);
		disconnect(unit, 0, this, 0);
	}
}

void NotificationFilterImpl::onNotificationFinished()
{
	Notification *notification = sender_cast<Notification*>(sender());
	ChatUnit *unit = getUnitForSession(notification->request().object());
	m_notifications.remove(unit, notification);
	if (!m_notifications.contains(unit))
		disconnect(unit, 0, this, 0);
}

void NotificationFilterImpl::onUnitDestroyed()
{
	m_notifications.remove(static_cast<ChatUnit*>(sender()));
}

void NotificationFilterImpl::onAccountCreated(qutim_sdk_0_3::Account *account)
{
	connect(account, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
			SLOT(onAccountStatusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)));
}

void NotificationFilterImpl::onAccountStatusChanged(const qutim_sdk_0_3::Status &status,
													const qutim_sdk_0_3::Status &previous)
{
	Account *acc = sender_cast<Account*>(sender());
	if (status.type() != Status::Offline && previous.type() == Status::Connecting) {
		QTimer *timer = m_connectingAccounts.value(acc);
		if (!timer) {
			timer = new QTimer(this);
			timer->setInterval(20000);
			timer->setSingleShot(true);
			timer->setProperty("account", qVariantFromValue(acc));
			connect(timer, SIGNAL(timeout()), SLOT(onAccountConnected()));
			m_connectingAccounts.insert(acc, timer);
		} else {
			timer->stop();
		}
		timer->start();
	}
}

void NotificationFilterImpl::onAccountConnected()
{
	QTimer *timer = sender_cast<QTimer*>(sender());
	Account *acc = timer->property("account").value<Account*>();
	Q_ASSERT(acc);
	timer->deleteLater();
	m_connectingAccounts.remove(acc);
}


} // namespace Core


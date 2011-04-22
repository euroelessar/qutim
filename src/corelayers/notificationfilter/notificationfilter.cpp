#include "notificationfilter.h"
#include <qutim/chatunit.h>
#include <qutim/chatsession.h>

namespace Core {

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
	case Notification::UserOnline:
		title = QObject::tr("%1 is online");
		break;
	case Notification::ChatUserLeaved:
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

NotificationFilterImpl::NotificationFilterImpl()
{
	registerFilter(this, LowPriority);
}

NotificationFilterImpl::~NotificationFilterImpl()
{
	unregisterFilter(this);
}

NotificationFilter::Result NotificationFilterImpl::filter(NotificationRequest &request)
{
	QObject *sender = request.object();
	if (!sender) {
		request.setTitle(toString(request.type()));
		return Accept;
	}

	if (request.title().isEmpty()) {
		QString sender_name;
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
		{
			NotificationAction action(QObject::tr("Open chat"),
									  this, SLOT(onOpenChatClicked(NotificationRequest)));
			request.addAction(action);
		}
		{
			NotificationAction action(QObject::tr("Ignore"),
									  this, SLOT(onIgnoreChatClicked(NotificationRequest)));
			request.addAction(action);
		}
		break;
	default:
		break;
	}
	return Accept;
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

} // namespace Core

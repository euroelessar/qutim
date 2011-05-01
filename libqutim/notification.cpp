/****************************************************************************
 *  notification.cpp
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

#include "notification.h"
#include "notificationslayer_p.h"
#include "dynamicpropertydata_p.h"
#include "message.h"
#include "chatunit.h"
#include "chatsession.h"
#include <QMetaEnum>
#include <QMetaMethod>
#include <QMultiMap>

namespace qutim_sdk_0_3 {

static SoundHandler handler; //TODO move to other place

typedef QList<NotificationBackend*> NotificationBackendList;
Q_GLOBAL_STATIC(NotificationBackendList, backendList)

typedef QHash<Notification::Type, NotificationAction> ActionHash;
Q_GLOBAL_STATIC(ActionHash, globalActions)

typedef QMultiMap<int, NotificationFilter*> HandlerMap;
Q_GLOBAL_STATIC(HandlerMap, handlers)

class NotificationPrivate
{
public:
	NotificationRequest request;
	QAtomicInt ref;
};

class NotificationRequestPrivate : public DynamicPropertyData
{
public:
	NotificationRequestPrivate() : DynamicPropertyData(), object(0) {}
	NotificationRequestPrivate(const NotificationRequestPrivate& o) :
		DynamicPropertyData(o), object(o.object), pixmap(o.pixmap), text(o.text),
		title(o.title), type(o.type), actions(o.actions) {}
	QObject *object;
	QPixmap pixmap;
	QString text;
	QString title;
	Notification::Type type;
	QList<NotificationAction> actions;
};

class NotificationActionPrivate : public QSharedData
{
public:
	QIcon icon;
	LocalizedString title;
	QPointer<QObject> receiver;
	QByteArray method;
	QPointer<Notification> notification;
};

Notification *Notification::send(const Message &msg)
{
	NotificationRequest request(msg);
	return request.send();
}

Notification::Notification(const NotificationRequest &request) :
	d_ptr(new NotificationPrivate)
{
	Q_D(Notification);
	d->request = request;

	QList<NotificationAction> &actions = d->request.d_ptr->actions;
	QList<NotificationAction>::iterator itr = actions.begin();
	while (itr != actions.end()) {
		itr->d->notification = this;
		++itr;
	}
}

Notification::~Notification()
{
}

NotificationRequest Notification::request() const
{
	return d_func()->request;
}

void Notification::accept()
{
	debug() << "Accepted";
	emit accepted();
	//deleteLater(); //TODO
}

LocalizedString Notification::typeString(Type type)
{
	QMetaObject meta = Notification::staticMetaObject;

	//convert flag to number
	int index = 0;
	int flag = 1;
	for (; index != meta.enumerator(0).keyCount(); index++) {
		if (flag == type)
			break;
		flag <<= 1;
	}

	LocalizedString strings[] =
	{
		QT_TRANSLATE_NOOP("Notification", "Incoming Message"),
		QT_TRANSLATE_NOOP("Notification", "Outgoing Message"),
		QT_TRANSLATE_NOOP("Notification", "qutIM Startup"),
		QT_TRANSLATE_NOOP("Notification", "Blocked Message"),
		QT_TRANSLATE_NOOP("Notification", "User joined chat"),
		QT_TRANSLATE_NOOP("Notification", "User leaved chat"),
		QT_TRANSLATE_NOOP("Notification", "Incoming chat message"),
		QT_TRANSLATE_NOOP("Notification", "Outgoing chat message"),
		QT_TRANSLATE_NOOP("Notification", "File transfer completed"),
		QT_TRANSLATE_NOOP("Notification", "User online"),
		QT_TRANSLATE_NOOP("Notification", "User offline"),
		QT_TRANSLATE_NOOP("Notification", "User changed status"),
		QT_TRANSLATE_NOOP("Notification", "User has birthday!"),
		QT_TRANSLATE_NOOP("Notification", "User typing"),
		QT_TRANSLATE_NOOP("Notification", "System")
	};
	return strings[index];
}

NotificationAction::NotificationAction(const QIcon &icon, const LocalizedString &title,
									   QObject *receiver, const char *method) :
	d(new NotificationActionPrivate)
{
	d->icon = icon;
	d->title = title;
	d->receiver = receiver;
	d->method = method;
}

NotificationAction::NotificationAction(const LocalizedString &title,
									   QObject *receiver, const char *method) :
	d(new NotificationActionPrivate)
{
	d->title = title;
	d->receiver = receiver;
	d->method = method;
}

NotificationAction::NotificationAction() :
	d(new NotificationActionPrivate)
{

}

NotificationAction::NotificationAction(const NotificationAction &action) :
	d(action.d)
{
}

NotificationAction::~NotificationAction()
{
}

const NotificationAction &NotificationAction::operator=(const NotificationAction &rhs)
{
	this->d = rhs.d;
	return *this;
}

QIcon NotificationAction::icon() const
{
	return d->icon;
}

LocalizedString NotificationAction::title() const
{
	return d->title;
}

QObject *NotificationAction::receiver() const
{
	return d->receiver;
}

const char *NotificationAction::method() const
{
	return d->method.constData();
}

void NotificationAction::trigger() const
{
	if (!d->receiver || !d->notification)
		return;

	const QMetaObject *meta = d->receiver->metaObject();
	const char *name = d->method.constData();
	const char type = name[0];
	++name;

	int index;
	switch (type) {
	case '0':
		index = meta->indexOfMethod(name);
		break;
	case '1':
		index = meta->indexOfSlot(name);
		break;
	case '2':
		index = meta->indexOfSignal(name);
		break;
	default:
		break;
	}

	if (index != -1)
		meta->method(index).invoke(d->receiver,
								   Q_ARG(NotificationRequest, d->notification->request()));
	else
		warning() << "Invalid action triggered";
}

namespace CompiledProperty
{
static QList<QByteArray> names = QList<QByteArray>() << "type"
													 << "text"
													 << "title"
													 << "object"
													 << "image"
													 << "actions";
static QList<Getter> getters   = QList<Getter>() //TODO
;
static QList<Setter> setters   = QList<Setter>() //TODO
;
}

NotificationRequest::NotificationRequest() :
	d_ptr(new NotificationRequestPrivate)
{

}

NotificationRequest::NotificationRequest(const Message &msg) :
	d_ptr(new NotificationRequestPrivate)
{
	d_ptr->text = msg.text();
	d_ptr->object = msg.chatUnit();

#if 1
	// TODO: remove that workaround when chat layer will be ported to new notification API
	QVariant service = msg.property("service");
	if (service.type() == QVariant::Int)
		d_ptr->type = static_cast<Notification::Type>(service.toInt());
	else
#endif

	if (ChatLayer::get(msg.chatUnit(), false))
		d_ptr->type = msg.isIncoming() ? Notification::ChatIncomingMessage :
										 Notification::ChatOutgoingMessage;
	else
		d_ptr->type = msg.isIncoming() ? Notification::IncomingMessage :
										 Notification::OutgoingMessage;

	setProperty("message", qVariantFromValue(msg));
}

NotificationRequest::NotificationRequest(Notification::Type type) :
	d_ptr(new NotificationRequestPrivate)
{
	d_ptr->type = type;
}

NotificationRequest::NotificationRequest(const NotificationRequest &other)
{
	d_ptr = other.d_ptr;
}

NotificationRequest::~NotificationRequest()
{
}

NotificationRequest &NotificationRequest::operator =(const NotificationRequest &other)
{
	d_ptr = other.d_ptr;
	return *this;
}

void NotificationRequest::setObject(QObject *obj)
{
	d_ptr->object = obj;
}

QObject *NotificationRequest::object() const
{
	return d_ptr->object;
}

void NotificationRequest::setImage(const QPixmap &pixmap)
{
	d_ptr->pixmap = pixmap;
}

QPixmap NotificationRequest::image() const
{
	return d_ptr->pixmap;
}

void NotificationRequest::setTitle(const QString &title)
{
	d_ptr->title = title;
}

QString NotificationRequest::title() const
{
	return d_ptr->title;
}

void NotificationRequest::setText(const QString &text)
{
	d_ptr->text = text;
}

QString NotificationRequest::text() const
{
	return d_ptr->text;
}

void NotificationRequest::setType(Notification::Type type)
{
	d_ptr->type = type;
}

Notification::Type NotificationRequest::type() const
{
	return d_ptr->type;
}

QVariant NotificationRequest::property(const char *name, const QVariant &def) const
{
	return d_ptr->property(name, def, CompiledProperty::names, CompiledProperty::getters);
}

void NotificationRequest::setProperty(const char *name, const QVariant &value)
{
	d_ptr->setProperty(name, value, CompiledProperty::names, CompiledProperty::setters);
}

void NotificationRequest::addAction(const NotificationAction &action_helper)
{
	NotificationAction action = action_helper;
	d_ptr->actions.push_back(action);
}

void NotificationRequest::addAction(Notification::Type type, const NotificationAction &action)
{
	globalActions()->insert(type, action);
}

QList<NotificationAction> NotificationRequest::actions() const
{
	QList<NotificationAction> actions = d_ptr->actions;
	actions += globalActions()->values(d_ptr->type);
	return actions;
}

Notification *NotificationRequest::send()
{
	HandlerMap::iterator itr = handlers()->end();
	HandlerMap::iterator begin = handlers()->begin();
	while (itr != begin) {
		--itr;
		NotificationFilter::Result res = (*itr)->filter(*this);
		if (res  == NotificationFilter::Error || res  == NotificationFilter::Reject)
			return 0;
	}

	Notification *notification = new Notification(*this);
	notification->d_func()->ref.ref();
	foreach (NotificationBackend *backend, *backendList())
		backend->handleNotification(notification);
	notification->d_func()->ref.deref();
	//TODO ref and deref impl
	return notification;
}

NotificationFilter::~NotificationFilter()
{

}

void NotificationFilter::registerFilter(NotificationFilter *handler, int priority)
{
	handlers()->insert(priority, handler);
}

void NotificationFilter::unregisterFilter(NotificationFilter *handler)
{
	HandlerMap::iterator itr = handlers()->begin();
	HandlerMap::iterator end = handlers()->end();
	while (itr != end) {
		if (*itr == handler)
			itr = handlers()->erase(itr);
		else
			break;
	}
}

NotificationBackend::NotificationBackend()
{
	backendList()->append(this);
}

NotificationBackend::~NotificationBackend()
{
	backendList()->removeAll(this);
}

void NotificationBackend::ref(Notification *notification)
{
	notification->d_func()->ref.ref();
}

void NotificationBackend::deref(Notification *notification)
{
	if (!notification->d_func()->ref.deref())
		notification->deleteLater();
}

} // namespace qutim_sdk_0_3

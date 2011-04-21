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
#include "dynamicpropertydata_p.h"
#include "message.h"
#include "chatunit.h"
#include "chatsession.h"
#include <QMetaEnum>

namespace qutim_sdk_0_3 {

typedef QList<NotificationBackend*> NotificationBackendList;
Q_GLOBAL_STATIC(NotificationBackendList, backendList)

class NotificationPrivate
{
public:
	NotificationRequest request;
	QAtomicInt ref;
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
}

Notification::~Notification()
{
}

NotificationRequest Notification::request() const
{
	return d_func()->request;
}

void Notification::close()
{
	emit finished();
	deleteLater();
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

class NotificationRequestPrivate : public DynamicPropertyData
{
public:
	NotificationRequestPrivate() : DynamicPropertyData() {}
	NotificationRequestPrivate(const NotificationRequestPrivate& o) :
		DynamicPropertyData(o), object(o.object),pixmap(o.pixmap),text(o.text),
		title(o.title),type(o.type) {}
	QObject *object;
	QPixmap pixmap;
	QString text;
	QString title;
	Notification::Type type;
	QList<const ActionGenerator*> actions;
};

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

void NotificationRequest::addAction(const ActionGenerator *action)
{
}

void NotificationRequest::addAction(Notification::Type type, const ActionGenerator *action)
{
}

QList<const ActionGenerator *> NotificationRequest::actions() const
{
	return d_ptr->actions;
}

Notification *NotificationRequest::send()
{
	//TODO Notification handlers

	Notification *notification = new Notification(*this);
	foreach (NotificationBackend *backend, *backendList())
		backend->handleNotification(notification);
	return notification;
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
	notification->d_func()->ref.deref();
}

} // namespace qutim_sdk_0_3

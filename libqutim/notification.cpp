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

namespace qutim_sdk_0_3 {
class NotificationPrivate
{
public:
	NotificationRequest request;
	QAtomicInt ref;
};

Notification::Notification(QObject *parent) : QObject(parent)
{
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

class NotificationRequestPrivate : public DynamicPropertyData
{
public:
	NotificationRequestPrivate() : DynamicPropertyData() {}
	NotificationRequestPrivate(const NotificationRequestPrivate& o) :
		DynamicPropertyData(o), object(o.object),pixmap(o.pixmap),text(o.text),
		title(o.title), data(o.data) {}
	QObject *object;
	QPixmap pixmap;
	QString text;
	QString title;
	QList<const ActionGenerator*> actions;
};

namespace CompiledProperty
{
static QList<QByteArray> names = QList<QByteArray>()
<< "text"
<< "title"
<< "object"
<< "image"
<< "actions";
static QList<Getter> getters   = QList<Getter>()
;
static QList<Setter> setters   = QList<Setter>()
;
}

NotificationRequest::NotificationRequest(const Message &msg) : d_ptr(new NotificationRequestPrivate)
{
	d_ptr->text = msg.text();
	d_ptr->object = msg.chatUnit();
	setProperty("message", qVariantFromValue(msg));
}

NotificationRequest::NotificationRequest(Notification::Type type) : d_ptr(new NotificationRequestPrivate)
{
}

NotificationRequest::NotificationRequest(const NotificationRequest &other)
{
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

NotificationBackend::NotificationBackend()
{
}

NotificationBackend::~NotificationBackend()
{
}

void NotificationBackend::ref(Notification *notification)
{
}

void NotificationBackend::deref(Notification *notification)
{
}
} // namespace qutim_sdk_0_3

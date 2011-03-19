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
	QObject *object;
	QPixmap pixmap;
	QString text;
	QString title;
	QVariant data;
};

NotificationRequest::NotificationRequest(const Message &msg) : d_ptr(new NotificationRequestPrivate)
{
}

NotificationRequest::NotificationRequest(Notification::Type type)
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
}

void NotificationRequest::setObject(QObject *obj)
{
	d_ptr->object = obj;
}

QObject *NotificationRequest::object() const
{
	return d_ptr->object;
}

void NotificationRequest::setData(const QVariant &data)
{
}

QVariant NotificationRequest::data() const
{
}

void NotificationRequest::setImage(const QPixmap &pixmap)
{
}

QPixmap NotificationRequest::image() const
{
}

void NotificationRequest::setTitle(const QString &title)
{
}

QString NotificationRequest::title() const
{
}

void NotificationRequest::setText(const QString &text)
{
}

QString NotificationRequest::text() const
{
}

QVariant NotificationRequest::property(const char *name, const QVariant &def) const
{
}

void NotificationRequest::setProperty(const char *name, const QVariant &value)
{
}

void NotificationRequest::addAction(const ActionGenerator *action)
{
}

void NotificationRequest::addAction(Notification::Type type, const ActionGenerator *action)
{
}

QList<const ActionGenerator *> NotificationRequest::actions() const
{
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

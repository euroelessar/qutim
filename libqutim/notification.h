/****************************************************************************
 *  notification.h
 *
 *  Copyright (c) 2011 by Sidorov Aleksey <sauron@citadelspb.com>
 *                     by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include "libqutim_global.h"
#include "actiongenerator.h"
#include <QIcon>

////Notifications API Draft
namespace qutim_sdk_0_3
{

class NotificationBackend;
class NotificationPrivate;
class NotificationRequest;
class NotificationRequestPrivate;
class Message;

class LIBQUTIM_EXPORT Notification : public QObject
{
    Q_OBJECT
	Q_DECLARE_PRIVATE(Notification)
	Q_ENUMS(Type)
public:
	enum Type
	{
		IncomingMessage,
		OutgoingMessage,
		AppStartup,
		BlockedMessage,
		ChatUserJoined,
		ChatUserLeaved,
		ChatIncomingMessage,
		ChatOutgoingMessage,
		FileTransferCompleted,
		UserOnline,
		UserOffline,
		UserChangedStatus,
		UserHasBirthday,
		UserTyping,
		System
	};
	static Notification *send(const Message &msg);
	~Notification();
	NotificationRequest request() const;
public slots:
	void close();
signals:
	void ignored();
	void finished();
protected:
	Notification(const NotificationRequest &request);
	QScopedPointer<NotificationPrivate> d_ptr;
	friend class NotificationRequest;
	friend class NotificationBackend;
};

class LIBQUTIM_EXPORT NotificationRequest
{
public:
	NotificationRequest();
	NotificationRequest(const Message &msg);
	NotificationRequest(Notification::Type type);
	NotificationRequest(const NotificationRequest &other);
	~NotificationRequest();
	NotificationRequest &operator =(const NotificationRequest &other);
	/*! Send new notification.
	  */
	Notification *send();
	void setObject(QObject *obj);
	QObject *object() const;
	void setImage(const QPixmap &pixmap);
	QPixmap image() const;
	void setTitle(const QString &title);
	QString title() const;
	void setText(const QString &text);
	QString text() const;
	QVariant property(const char *name, const QVariant &def) const;
	template<typename T>
	T property(const char *name, const T &def) const
	{ return qVariantValue<T>(property(name, qVariantFromValue<T>(def))); }
	void setProperty(const char *name, const QVariant &value);
	void addAction(const ActionGenerator *action);
	static void addAction(Notification::Type type, const ActionGenerator *action);
	QList<const ActionGenerator *> actions() const;
private:
	QSharedDataPointer<NotificationRequestPrivate> d_ptr;
};

class LIBQUTIM_EXPORT NotificationHandler
{
public:
	enum Result
	{
		Accept,
		Reject,
		Error
	};
	enum Priority
	{
		LowPriority       = 0x00000100,
		NormalPriortity   = 0x00010000,
		HighPriority      = 0x01000000
	};
	virtual ~NotificationHandler();

	static void registerHandler(NotificationHandler *handler,
								int priority = NormalPriortity);
	static void unregisterHandler(NotificationHandler *handler);
	static Result handle(NotificationRequest &request);
protected:
	virtual Result doHandle() = 0;
};

class LIBQUTIM_EXPORT NotificationBackend : public QObject
{
	Q_OBJECT
	Q_CLASSINFO("Type", "NoType")
public:
	NotificationBackend();
	~NotificationBackend();
	virtual void handleNotification(Notification *notification) = 0;
protected:
	void ref(Notification *notification);
	void deref(Notification *notification);
};

} // namespace qutim_sdk_0_3

//Q_ENUMS(qutim_sdk_0_3::Notification::Type)

#endif // NOTIFICATION_H

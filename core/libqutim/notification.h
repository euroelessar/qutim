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
class NotificationActionPrivate;
class NotificationBackendPrivate;
class Message;

class LIBQUTIM_EXPORT Notification : public QObject
{
    Q_OBJECT
	Q_DECLARE_PRIVATE(Notification)
	Q_ENUMS(Type)
public:
	typedef QSharedPointer<Notification> Ptr; //FIXME find usable pointer!
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
		System,
		LastType = System
	};
	static Notification *send(const Message &msg);
	static Notification *send(const QString &text);
	~Notification();
	NotificationRequest request() const;
	static LocalizedString typeString(Type type);
	static LocalizedStringList typeStrings();
public slots:
	void accept();
	void ignore();
signals:
	void accepted();
	void ignored();
protected:
	Notification(const NotificationRequest &request);
	QScopedPointer<NotificationPrivate> d_ptr;
	friend class NotificationRequest;
	friend class NotificationBackend;
};
typedef QList<Notification*> NotificationList;

class LIBQUTIM_EXPORT NotificationAction
{
public:
	enum Type
	{
		AcceptButton,
		IgnoreButton,
		AdditionalButton
	};

	NotificationAction();
	NotificationAction(const QIcon &icon, const LocalizedString &title,
					   QObject *receiver, const char *method);
	NotificationAction(const LocalizedString &title,
					   QObject *receiver, const char *method);
	NotificationAction(const NotificationAction &action);
	~NotificationAction();
	const NotificationAction &operator=(const NotificationAction &rhs);
	QIcon icon() const;
	LocalizedString title() const;
	Type type() const;
	void setType(Type type);
	QObject *receiver() const;
	const char *method() const;
	void trigger() const;
private:
	friend class Notification;
	QSharedDataPointer<NotificationActionPrivate> d;
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
	void setType(Notification::Type type);
	Notification::Type type() const;
	void setBackends(const QSet<QByteArray> &backendTypes);
	void blockBackend(const QByteArray &backendType);
	void unblockBackend(const QByteArray &backendType);
	bool isBackendBlocked(const QByteArray &backendType);
	static void blockBackend(Notification::Type type, const QByteArray &backendType);
	static void unblockBackend(Notification::Type type, const QByteArray &backendType);
	static bool isBackendBlocked(Notification::Type type, const QByteArray &backendType);
	QVariant property(const char *name, const QVariant &def) const;
	template<typename T>
	T property(const char *name, const T &def) const
	{ return qVariantValue<T>(property(name, qVariantFromValue<T>(def))); }
	void setProperty(const char *name, const QVariant &value);
	void addAction(const NotificationAction &action);
	static void addAction(Notification::Type type, const NotificationAction &action);
	QList<NotificationAction> actions() const;
private:
	friend class Notification;
	QSharedDataPointer<NotificationRequestPrivate> d_ptr;
};

class LIBQUTIM_EXPORT NotificationFilter
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
	virtual ~NotificationFilter();

	static void registerFilter(NotificationFilter *handler,
								int priority = NormalPriortity);
	static void unregisterFilter(NotificationFilter *handler);
protected:
	friend class NotificationRequest;
	virtual Result filter(NotificationRequest &request) = 0;
	virtual void virtual_hook(int id, void *data);
};

class LIBQUTIM_EXPORT NotificationBackend
{
	Q_DECLARE_PRIVATE(NotificationBackend)
public:
	NotificationBackend(const QByteArray &type);
	virtual ~NotificationBackend();
	//TODO rewrite on Notification::Ptr
	virtual void handleNotification(Notification *notification) = 0;
	QByteArray backendType() const;
	LocalizedString description() const;
	static QList<QByteArray> allTypes();
	static NotificationBackend* get(const QByteArray &type);
	static QList<NotificationBackend*> all();
protected:
	void ref(Notification *notification);
	void deref(Notification *notification);
	void setDescription(const LocalizedString &description);
	virtual void virtual_hook(int id, void *data);
private:
	QScopedPointer<NotificationBackendPrivate> d_ptr;
};

} // namespace qutim_sdk_0_3

Q_DECLARE_METATYPE(qutim_sdk_0_3::Notification*)
Q_DECLARE_METATYPE(qutim_sdk_0_3::NotificationRequest)
Q_DECLARE_METATYPE(qutim_sdk_0_3::NotificationAction)
Q_DECLARE_INTERFACE(qutim_sdk_0_3::NotificationFilter, "org.qutim.core.NotificationFilter")
Q_DECLARE_INTERFACE(qutim_sdk_0_3::NotificationBackend, "org.qutim.core.NotificationBackend")

#endif // NOTIFICATION_H

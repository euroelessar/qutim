/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include "libqutim_global.h"
#include "actiongenerator.h"
#include <QIcon>

namespace qutim_sdk_0_3
{

class NotificationBackend;
class NotificationPrivate;
class NotificationRequest;
class NotificationRequestPrivate;
class NotificationActionPrivate;
class NotificationBackendPrivate;
class Message;
class Status;
class Buddy;

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
		ChatUserLeft,
		ChatIncomingMessage,
		ChatOutgoingMessage,
		FileTransferCompleted,
		UserOnline,
		UserOffline,
		UserChangedStatus,
		UserHasBirthday,
		UserTyping,
		System,
		Attention,
		LastType = Attention
	};
	enum State
	{
		Active,
		Accepted,
		Ignored,
		Rejected
	};
	static Notification *send(const Message &msg);
	static Notification *send(const QString &text);
	~Notification();
	NotificationRequest request() const;
	State state();
	static LocalizedString typeString(Type type);
	static LocalizedStringList typeStrings();
	static LocalizedString descriptionString(Type type);
	static LocalizedStringList descriptionStrings();
public slots:
	void accept();
	void ignore();
	void reject();
signals:
	void accepted();
	void ignored();
	void rejected();
	void finished(qutim_sdk_0_3::Notification::State state);
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
	NotificationRequest(Buddy *buddy, const Status &status, const Status &previous);
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
	void reject(const QByteArray &reason);
	QSet<QByteArray> rejectionReasons() const;
	void setBackends(const QSet<QByteArray> &backendTypes);
	void blockBackend(const QByteArray &backendType);
	void unblockBackend(const QByteArray &backendType);
	bool isBackendBlocked(const QByteArray &backendType);
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
	virtual void filter(NotificationRequest &request) = 0;
	virtual void notificationCreated(Notification *notification);
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
	void allowRejectedNotifications(const QByteArray &reason);
	virtual void virtual_hook(int id, void *data);
private:
	friend class NotificationRequest;
	QScopedPointer<NotificationBackendPrivate> d_ptr;
};

class LIBQUTIM_EXPORT NotificationManager : public QObject
{
	Q_OBJECT
public:
	static NotificationManager *instance();
	static void setBackendState(const QByteArray &type, bool enabled);
	static void enableBackend(const QByteArray &type);
	static void disableBackend(const QByteArray &type);
	static bool isBackendEnabled(const QByteArray &type);
signals:
	void backendCreated(const QByteArray &type, qutim_sdk_0_3::NotificationBackend *backend);
	void backendDestroyed(const QByteArray &type, qutim_sdk_0_3::NotificationBackend *backend);
	void backendStateChanged(const QByteArray &type, bool enabled);
private:
	friend class NotificationBackend;
	NotificationManager();
};

} // namespace qutim_sdk_0_3

Q_DECLARE_METATYPE(qutim_sdk_0_3::Notification*)
Q_DECLARE_METATYPE(qutim_sdk_0_3::NotificationRequest)
Q_DECLARE_METATYPE(qutim_sdk_0_3::NotificationAction)
Q_DECLARE_METATYPE(qutim_sdk_0_3::NotificationBackend*)
Q_DECLARE_INTERFACE(qutim_sdk_0_3::NotificationFilter, "org.qutim.core.NotificationFilter")

#endif // NOTIFICATION_H


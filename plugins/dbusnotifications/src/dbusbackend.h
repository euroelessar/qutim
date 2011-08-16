#ifndef GROWLBACKEND_H
#define GROWLBACKEND_H

#include <qutim/notification.h>
#include <qutim/sound.h>
#include "org_freedesktop_notification.h"

QDBusArgument &operator<< (QDBusArgument &arg, const QImage &image);
const QDBusArgument &operator>> (const QDBusArgument &arg, QImage &image);

class DBusBackend : public QObject, public qutim_sdk_0_3::NotificationBackend
{
	Q_OBJECT
	Q_CLASSINFO("Service", "Popup")
public:
	DBusBackend();
	virtual ~DBusBackend();
	virtual void handleNotification(qutim_sdk_0_3::Notification *notification);
protected slots:
	void callFinished(QDBusPendingCallWatcher* watcher);
	void capabilitiesCallFinished(QDBusPendingCallWatcher* watcher);
	void onActionInvoked(quint32 id, const QString &action_key);
	void onNotificationClosed(quint32 id, quint32 reason);
private:
	struct NotificationData
	{
		QPointer<QObject> sender;
		QString body;
		QList<QPointer<qutim_sdk_0_3::Notification> > notifications;
		QMultiHash<QString, qutim_sdk_0_3::NotificationAction> actions;
	};
	void ignore(NotificationData &data);
private:
	QScopedPointer<org::freedesktop::Notifications> interface;
	QHash<quint32, NotificationData> m_notifications;
	QHash<QObject*, quint32> m_ids;
	QSet<QString> m_capabilities;


};

#endif // GROWLBACKEND_H

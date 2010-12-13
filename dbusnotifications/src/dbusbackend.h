#ifndef GROWLBACKEND_H
#define GROWLBACKEND_H

#include <qutim/notificationslayer.h>
#include "notification.h"
#include <QDBusArgument>

#ifdef Q_WS_MAEMO_5
#include <mce/mode-names.h>
#include <mce/dbus-names.h>
#endif

QDBusArgument& operator<< (QDBusArgument& arg, const QImage& image);
const QDBusArgument& operator>> (const QDBusArgument& arg, QImage& image);

class DBusBackend : public qutim_sdk_0_3::PopupBackend
{
	Q_OBJECT
public:
	virtual void show(qutim_sdk_0_3::Notifications::Type type, QObject* sender,
					  const QString& body, const QVariant& data);
	DBusBackend();
	virtual ~DBusBackend();
protected slots:
	void callFinished(QDBusPendingCallWatcher* watcher);
	void loadSettings();
	void onActionInvoked(quint32 id, const QString &action_key);
	void onNotificationClosed(quint32 id);
	void enableVibration();
	void stopVibration();
	void vibrate(int aTimeout);

private:
	QScopedPointer<org::freedesktop::Notifications> interface;
	int m_showFlags;
	struct NotificationData
	{
		QObject *sender;
		QString body;
		QVariant data;
	};

	QHash<quint32, NotificationData> m_senders;
	QDBusInterface *mDbusInterface;

};

#endif // GROWLBACKEND_H

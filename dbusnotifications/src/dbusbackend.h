#ifndef GROWLBACKEND_H
#define GROWLBACKEND_H

#include <qutim/notificationslayer.h>
#include "notification.h"
#include <QDBusArgument>

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
private:
	QScopedPointer<org::freedesktop::Notifications> interface;
	int m_showFlags;
};

#endif // GROWLBACKEND_H

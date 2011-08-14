#ifndef GROWLBACKEND_H
#define GROWLBACKEND_H

#include <qutim/notification.h>
class GrowlNotificationWrapper;
class GrowlBackend : public QObject, public qutim_sdk_0_3::NotificationBackend
{
	Q_OBJECT
	Q_CLASSINFO("Service", "Popup")
	Q_CLASSINFO("Type", "Popup")
public:
	GrowlBackend();
	virtual ~GrowlBackend();
protected:
	virtual void handleNotification(qutim_sdk_0_3::Notification *notification);
private:
	QScopedPointer<GrowlNotificationWrapper> m_wrapper;
};

#endif // GROWLBACKEND_H

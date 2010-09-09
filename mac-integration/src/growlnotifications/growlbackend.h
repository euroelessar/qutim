#ifndef GROWLBACKEND_H
#define GROWLBACKEND_H

#include <qutim/notificationslayer.h>
class GrowlNotificationWrapper;
class GrowlBackend : public qutim_sdk_0_3::PopupBackend
{
	Q_OBJECT
public:
	virtual void show(qutim_sdk_0_3::Notifications::Type type, QObject* sender,
					  const QString& body, const QVariant& data);
	GrowlBackend();
	virtual ~GrowlBackend();
private:
	QScopedPointer<GrowlNotificationWrapper> m_wrapper;
};

#endif // GROWLBACKEND_H

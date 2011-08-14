#ifndef RESOURCEICONLOADER_H
#define RESOURCEICONLOADER_H
#include <qutim/notification.h>
#include <QStringList>
#include <hwrmvibra.h>

namespace Symbian
{
using namespace qutim_sdk_0_3;

class Vibration : public QObject, public qutim_sdk_0_3::NotificationBackend
{
	Q_OBJECT
	Q_CLASSINFO("Service", "VibrationAlert")
	Q_CLASSINFO("Type", "VibrationAlert")
public:
	Vibration();
	~Vibration();

	virtual void handleNotification(qutim_sdk_0_3::Notification *notification);
private:
	CHWRMVibra *m_vibra;
};

} //namespace Embedded

#endif // RESOURCEICONLOADER_H

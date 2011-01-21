#ifndef NOTIFICATIONSSETTINGS_H
#define NOTIFICATIONSSETTINGS_H

#include <QObject>
#include <qutim/startupmodule.h>

namespace Core
{

	class NotificationsSettings : public QObject, public qutim_sdk_0_3::StartupModule
	{
		Q_OBJECT
		Q_INTERFACES(qutim_sdk_0_3::StartupModule)
	public:
		explicit NotificationsSettings(QObject *parent = 0);
	};

}

#endif // NOTIFICATIONSSETTINGS_H

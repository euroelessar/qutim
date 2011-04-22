#ifndef NOTIFICATIONFILTER_H
#define NOTIFICATIONFILTER_H

#include <qutim/notification.h>
#include <qutim/startupmodule.h>

namespace Core {

using namespace qutim_sdk_0_3;

class NotificationFilterImpl : public QObject, public NotificationFilter, public StartupModule
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::NotificationFilter qutim_sdk_0_3::StartupModule)
public:
	NotificationFilterImpl();
	virtual ~NotificationFilterImpl();
	virtual Result filter(NotificationRequest &request);
private slots:
	void onOpenChatClicked(const NotificationRequest &request);
	void onIgnoreChatClicked(const NotificationRequest &request);
};

} // namespace Core

#endif // NOTIFICATIONFILTER_H

/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
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


#ifndef NOTIFICATIONFILTER_H
#define NOTIFICATIONFILTER_H

#include <qutim/notification.h>
#include <qutim/startupmodule.h>
#include <qutim/chatsession.h>
#include <QMultiHash>
#include <QTimer>

namespace Core {

using namespace qutim_sdk_0_3;

class NotificationFilterImpl : public QObject, public qutim_sdk_0_3::NotificationFilter, public qutim_sdk_0_3::StartupModule
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::NotificationFilter qutim_sdk_0_3::StartupModule)
public:
	NotificationFilterImpl();
	virtual ~NotificationFilterImpl();
	virtual void filter(NotificationRequest &request);
	virtual void notificationCreated(Notification *notification);
private slots:
	void onOpenChatClicked(const qutim_sdk_0_3::NotificationRequest &request);
	void onIgnoreChatClicked(const qutim_sdk_0_3::NotificationRequest &request);
	void onSessionCreated(qutim_sdk_0_3::ChatSession *session);
	void onSessionActivated(bool active);
	void onNotificationFinished();
	void onUnitDestroyed();
	void onAccountCreated(qutim_sdk_0_3::Account *account);
	void onAccountStatusChanged(const qutim_sdk_0_3::Status &status,
								const qutim_sdk_0_3::Status &previous);
	void onAccountConnected();
private:
	typedef QMultiHash<ChatUnit*, QPointer<Notification> > Notifications;
	Notifications m_notifications;
	QHash<Account*, QTimer*> m_connectingAccounts;
};

} // namespace Core

#endif // NOTIFICATIONFILTER_H


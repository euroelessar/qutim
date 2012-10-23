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

using namespace Ureen;

class NotificationFilterImpl : public QObject, public Ureen::NotificationFilter, public Ureen::StartupModule
{
	Q_OBJECT
	Q_INTERFACES(Ureen::NotificationFilter Ureen::StartupModule)
public:
	NotificationFilterImpl();
	virtual ~NotificationFilterImpl();
	virtual void filter(NotificationRequest &request);
	virtual void notificationCreated(Notification *notification);
private slots:
	void onOpenChatClicked(const Ureen::NotificationRequest &request);
	void onIgnoreChatClicked(const Ureen::NotificationRequest &request);
	void onSessionCreated(Ureen::ChatSession *session);
	void onSessionActivated(bool active);
	void onNotificationFinished();
	void onUnitDestroyed();
	void onAccountCreated(Ureen::Account *account);
	void onAccountStatusChanged(const Ureen::Status &status,
								const Ureen::Status &previous);
	void onAccountConnected();
private:
	typedef QMultiHash<ChatUnit*, QWeakPointer<Notification> > Notifications;
	Notifications m_notifications;
	QHash<Account*, QTimer*> m_connectingAccounts;
};

} // namespace Core

#endif // NOTIFICATIONFILTER_H


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

#ifndef CORE_CHATNOTIFICATIONSBACKEND_H
#define CORE_CHATNOTIFICATIONSBACKEND_H

#include <qutim/notification.h>
#include <qutim/startupmodule.h>
#include <qutim/chatsession.h>

namespace Core {

using namespace qutim_sdk_0_3;

class ChatNotificationsBackend : public QObject, public qutim_sdk_0_3::StartupModule, public qutim_sdk_0_3::NotificationBackend
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::StartupModule)
public:
    ChatNotificationsBackend();
protected:
	virtual void handleNotification(qutim_sdk_0_3::Notification *notification);
private slots:
	void onSessionCreated(qutim_sdk_0_3::ChatSession *session);
	void onSessionActivated(bool active);
	void onUnitDestroyed();
private:
	QPointer<qutim_sdk_0_3::ChatSession> m_currentSession;
	QMap<ChatUnit *, MessageList> m_unaddedMessages;
};

} // namespace Core

#endif // CORE_CHATNOTIFICATIONSBACKEND_H


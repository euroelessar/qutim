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

using namespace Ureen;

class ChatNotificationsBackend : public QObject, public Ureen::StartupModule, public Ureen::NotificationBackend
{
	Q_OBJECT
	Q_INTERFACES(Ureen::StartupModule)
public:
    ChatNotificationsBackend();
protected:
	virtual void handleNotification(Ureen::Notification *notification);
private slots:
	void onSessionCreated(Ureen::ChatSession *session);
	void onSessionActivated(bool active);
	void onUnitDestroyed();
private:
	QWeakPointer<Ureen::ChatSession> m_currentSession;
	QMap<ChatUnit *, MessageList> m_unaddedMessages;
};

} // namespace Core

#endif // CORE_CHATNOTIFICATIONSBACKEND_H


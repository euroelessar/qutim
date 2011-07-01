/****************************************************************************
 *  chatnotificationsbackend.h
 *
 *  Copyright (c) 2011 by Prokhin Alexey <alexey.prokhin@yandex.ru>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

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
	QWeakPointer<qutim_sdk_0_3::ChatSession> m_currentSession;
	QMap<ChatUnit *, MessageList> m_unaddedMessages;
};

} // namespace Core

#endif // CORE_CHATNOTIFICATIONSBACKEND_H

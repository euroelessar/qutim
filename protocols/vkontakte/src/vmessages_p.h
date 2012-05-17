/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#ifndef VMESSAGES_P_H
#define VMESSAGES_P_H

#include <QObject>
#include "vkontakte_global.h"
#include <QTimer>
#include <qutim/message.h>

class VMessages;
class VConnection;

namespace qutim_sdk_0_3
{
	class ChatSession;
}

class VMessagesPrivate : public QObject
{
	Q_OBJECT
	Q_DECLARE_PUBLIC(VMessages)
public:
	VConnection *connection;
	VMessages *q_ptr;
	QHash<ChatSession*, MessageList> unreadMessages;
public slots:
	void onConnectStateChanged(VConnectionState state);
	void onHistoryReceived(); //TODO move to vconnection
	void onMessageSended();
	void onUnreadChanged(const qutim_sdk_0_3::MessageList &list);
};

#endif // VMESSAGES_P_H


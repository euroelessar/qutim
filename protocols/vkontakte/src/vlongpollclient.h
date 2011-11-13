/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef VLONGPOLLCLIENT_H
#define VLONGPOLLCLIENT_H

#include <QObject>
#include <QUrl>
#include "vkontakte_global.h"
#include <qutim/message.h>

class VConnection;

namespace qutim_sdk_0_3
{
	class ChatSession;
}

//TODO make pimpl interface
class VLongPollClient : public QObject
{
	Q_OBJECT
public:
	enum ServerAnswer
	{
		MessageDeleted      = 0,
		MessageFlagsReplaced= 1,
		MessageFlagsSet     = 2,
		MessageFlagsReseted = 3,
		MessageAdded		= 4,
		UserOnline          = 8,
		UserOffline         = 9
	};
	
	enum MessageFlag
	{
		MessageUnread   = 1,
		MessageOutbox   = 2,
		MessageReplied  = 4,
		MessageImportant= 8,
		MessageChat     = 16,
		MessageFriends  = 32,
		MessageSpam     = 64,
		MessageDeletedF = 128,
		MessageFixed    = 256,
		MessageMedia    = 512
	};
	Q_DECLARE_FLAGS(MessageFlags, MessageFlag)
	
	enum OfflineFlag
	{
		OfflineTimeout = 1
	};
	Q_DECLARE_FLAGS(OfflineFlags, OfflineFlag)
	
    VLongPollClient(VConnection *connection);
	virtual ~VLongPollClient();
	
protected slots:
	void requestServer();
	void requestData(const QString &ts);
	void onConnectionStateChanged(VConnectionState state);
	void onServerDataReceived();
	void onDataReceived();
	void onUnreadChanged(const qutim_sdk_0_3::MessageList &list);
private:
	VConnection *m_connection;
	QUrl m_url;
	QHash<ChatSession*,MessageList>m_unread_mess;
};

#endif // VLONGPOLLCLIENT_H


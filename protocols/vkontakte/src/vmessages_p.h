/****************************************************************************
 *  vmessages_p.h
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
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
	QHash<ChatSession*,MessageList> unreadMess;
public slots:
	void onConnectStateChanged(VConnectionState state);
	void onHistoryRecieved(); //TODO move to vconnection
	void onMessagesRecieved();
	void onMessageSended();
	void onSmsSended();
	void onUnreadChanged(const qutim_sdk_0_3::MessageList &list);
};

#endif // VMESSAGES_P_H

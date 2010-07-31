/****************************************************************************
 *  vmessages_p.h
 *
 *  Copyright (c) 2010 by Aleksey Sidorov <sauron@citadelspb.com>
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

class VMessages;
class VConnection;

class VMessagesPrivate : public QObject
{
	Q_OBJECT
	Q_DECLARE_PUBLIC(VMessages)
public:
	VConnection *connection;
	VMessages *q_ptr;
	int unreadMessageCount;
//	QTimer historyTimer;
public slots:
	void onConnectStateChanged(VConnectionState state);
	void onHistoryRecieved(); //TODO move to vconnection
	void onMessagesRecieved();
	void onMessageSended();
};

#endif // VMESSAGES_P_H

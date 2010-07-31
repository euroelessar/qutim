/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#ifndef VLONGPOLLCLIENT_H
#define VLONGPOLLCLIENT_H

#include <QObject>
#include <QUrl>
#include "vkontakte_global.h"

class VConnection;

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
	
private:
	VConnection *m_connection;
	QUrl m_url;
};

#endif // VLONGPOLLCLIENT_H

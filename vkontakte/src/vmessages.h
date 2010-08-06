/****************************************************************************
 *  vmessages.h
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

#ifndef VMESSAGES_H
#define VMESSAGES_H

#include <QObject>
#include "vkontakte_global.h"

namespace qutim_sdk_0_3 {
	class Config;
	class Message;
}

class VMessagesPrivate;
class VConnection;

class VMessages : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(VMessages)
public:
	VMessages(VConnection *connection,QObject* parent = 0);
	virtual ~VMessages();
	void getLastMessages(int count);
	void sendMessage(const Message &message);
	void sendSms(const Message &message);
	void markAsRead(const QStringList &messages);
	Config config();
public slots:
	void getHistory();
	void loadSettings();
	void saveSettings();
private:
	QScopedPointer<VMessagesPrivate> d_ptr;
};

#endif // VMESSAGES_H

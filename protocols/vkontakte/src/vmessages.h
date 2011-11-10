/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Sidorov Aleksey <sauron@citadelspb.com>
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


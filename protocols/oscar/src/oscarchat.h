/****************************************************************************
**
** qutIM instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@ya.ru>
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

#ifndef OSCARCHAT_H
#define OSCARCHAT_H

#include <qutim/conference.h>
#include "chatconnection.h"

namespace qutim_sdk_0_3 {
namespace oscar {

class OscarChat : public Conference
{
    Q_OBJECT
public:
    explicit OscarChat(const RoomId &id, ChatNavigation *navigation);
	~OscarChat();

	QString id() const;
	QString title() const;
	virtual Buddy *me() const;
	void attach(ChatConnection *connection);
	virtual bool sendMessage(const qutim_sdk_0_3::Message &message);
	QList<ChatUnit*> lowerUnits();

public slots:
	void doJoin();
	void doLeave();
	void invite(qutim_sdk_0_3::Contact *contact, const QString &reason = QString());

protected slots:
	void onUsersJoined(const QList<qutim_sdk_0_3::oscar::IcqContact*> &contacts);
	void onUsersLeaved(const QList<qutim_sdk_0_3::oscar::IcqContact*> &contacts);
	
private:
	QString m_id;
	RoomId m_roomId;
	QWeakPointer<ChatConnection> m_connection;
	ChatNavigation *m_navigation;
};

}
}

#endif // OSCARCHAT_H

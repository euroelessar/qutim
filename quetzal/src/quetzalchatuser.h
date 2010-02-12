/****************************************************************************
 *  quetzalchatuser.h
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#ifndef QUETZALCHATUSER_H
#define QUETZALCHATUSER_H

#include <purple.h>
#include <qutim/chatunit.h>

using namespace qutim_sdk_0_3;
class QuetzalChat;

class QuetzalChatUser : public ChatUnit
{
Q_OBJECT
public:
	explicit QuetzalChatUser(PurpleConvChatBuddy *user, QuetzalChat *chat);

	virtual QString id() const { return m_id; }
	virtual QString title() const { return m_name.isEmpty() ? m_id : m_name; }
	virtual void sendMessage(const Message &message);

	void update();
	void rename(const QString &id, const QString &name);
private:
	PurpleConvChatBuddy *m_user;
	QString m_id;
	QString m_name;
};

#endif // QUETZALCHATUSER_H

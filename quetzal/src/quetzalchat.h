/****************************************************************************
 *  quetzalchat.h
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

#ifndef QUETZALCHAT_H
#define QUETZALCHAT_H

#include "quetzalconverstion.h"
#include "quetzalchatuser.h"
#include <qutim/conference.h>

class QuetzalChat : public qutim_sdk_0_3::Conference
{
	Q_OBJECT
public:
	explicit QuetzalChat(PurpleConversation *conv);
	QString id() const;
	QString title() const;
	virtual QString topic() const;
	virtual void setTopic(const QString &topic);
	void addUsers(GList *cbuddies, gboolean new_arrivals);
	virtual qutim_sdk_0_3::Buddy *me() const;
	void setMe(const char *nick);
	virtual void join();
	virtual void leave();
	PurpleConversation *purple() { return m_conv; }
	void renameUser(const char *old_name, const char *new_name, const char *new_alias);
	void removeUsers(GList *users);
	void updateUser(const char *user);
	virtual bool sendMessage(const Message &message);
	void update(PurpleConvUpdateType type);
	void invite(qutim_sdk_0_3::Contact *contact, const QString &reason = QString());
private:
	PurpleConversation *m_conv;
	QString m_nick;
	QString m_id;
	QString m_title;
	QString m_topic;
	QHash<QString, QuetzalChatUser*> m_users;
};

#endif // QUETZALCHAT_H

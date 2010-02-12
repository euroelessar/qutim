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

class QuetzalChat : public QuetzalConversation
{
Q_OBJECT
public:
	explicit QuetzalChat(PurpleConversation *conv);
	void addUsers(GList *cbuddies, gboolean new_arrivals);
	QuetzalChatUser *getUser(const char *who) { return m_users.value(who); }
	void renameUser(const char *old_name, const char *new_name, const char *new_alias);
	void removeUsers(GList *users);
	void updateUser(const char *user);
private:
	QHash<QString, QuetzalChatUser *> m_users;
};

#endif // QUETZALCHAT_H

/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#ifndef VROSTER_H
#define VROSTER_H

#include <QObject>
#include <QHash>
#include <vk/contact.h>
#include <qutim/status.h>
#include <QTimer>

namespace vk {
class Roster;
class Contact;
class Buddy;
class Message;
}
class VAccount;
class VContact;
class VGroupChat;
class VContactsFactory;

class VRoster : public QObject
{
	Q_OBJECT
public:
	VRoster(VAccount *account);
	VContact *contact(int id, bool create = true);
	VContact *contact(int id) const;
	VGroupChat *groupChat(int id, bool create = true);
	VGroupChat *groupChat(int id) const;
protected slots:
	VContact *createContact(vk::Buddy *buddy);
private slots:
	void onContactDestroyed(QObject *obj);
	void onGroupChatDestroyed(QObject *obj);
	void onAddFriend(vk::Buddy *buddy);

	void onMessageAdded(const vk::Message &msg);
	void onContactTyping(int userId, int chatId);
private:
	VAccount *m_account;
	vk::Roster *m_roster;
	QHash<int, VContact*> m_contactHash;
	QHash<int, VGroupChat*> m_groupChatHash;
};


#endif // VROSTER_H


/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#ifndef VGROUPCHAT_H
#define VGROUPCHAT_H
#include <qutim/conference.h>
#include <qutim/chatsession.h>
#include <vreen/message.h>
#include "vaccount.h"

namespace Vreen {
class GroupChatSession;
class Buddy;
}

class VGroupChat : public qutim_sdk_0_3::Conference
{
	Q_OBJECT
public:
	VGroupChat(VAccount *account, int chatId);
	~VGroupChat();
	void setTyping(int uid, bool set);
	VContact *findContact(int uid) const;

	virtual qutim_sdk_0_3::Buddy *me() const;
	virtual bool sendMessage(const qutim_sdk_0_3::Message &message);
	virtual QString id() const;
	virtual QString title() const;
	virtual qutim_sdk_0_3::ChatUnitList lowerUnits();
	qutim_sdk_0_3::ChatUnit *findParticipant(int uid) const;
	Vreen::GroupChatSession *chatSession() const;
public slots:
	void handleMessage(const Vreen::Message &message);
protected:
	virtual void doJoin();
	virtual void doLeave();
protected slots:
	void onBuddyAdded(Vreen::Buddy *buddy);
	void onBuddyRemoved(Vreen::Buddy *buddy);
	void onUserDestroyed(QObject *obj);
	void onJoinedChanged(bool set);
	void onTitleChanged(const QString &title);
	void onMessageGet(const QVariant &response);
	void onMessageSent(const QVariant &response);
	void onUnreadChanged(qutim_sdk_0_3::MessageList unread);
	void onSessionCreated(qutim_sdk_0_3::ChatSession *session);
private:
	VAccount *m_account;
	Vreen::GroupChatSession *m_chatSession;
	QHash<Vreen::Buddy*, VContact*> m_buddies;
	QString m_title;

	//TODO rewrite on unite message handler
	qutim_sdk_0_3::MessageList m_unreadMessages;
	uint m_unreachedMessagesCount;
	typedef QList<QPair<int, int> > SentMessagesList;
	SentMessagesList m_sentMessages;
	Vreen::MessageList m_pendingMessages;
};

#endif // VGROUPCHAT_H

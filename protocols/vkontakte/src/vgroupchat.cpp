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

#include "vgroupchat.h"
#include <vk/groupchatsession.h>
#include <vcontact.h>

using namespace qutim_sdk_0_3;

VGroupChat::VGroupChat(VAccount *account, int chatId) :
    Conference(account),
    m_chatSession(new vk::GroupChatSession(chatId, account->client()))
{
    m_chatSession->setParent(this);
    connect(m_chatSession, SIGNAL(participantAdded(vk::Contact*)), SLOT(onBuddyAdded(vk::Buddy*)));
    connect(m_chatSession, SIGNAL(participantRemoved(vk::Contact*)), SLOT(onBuddyRemoved(vk::Buddy*)));
    connect(account->client(), SIGNAL(onlineStateChanged(bool)), SLOT(setJoined(bool)));
}

void VGroupChat::doJoin()
{
    m_chatSession->addParticipant(account()->client()->me());
}

void VGroupChat::doLeave()
{
    m_chatSession->removeParticipant(account()->client()->me());
}

void VGroupChat::onBuddyAdded(vk::Buddy *buddy)
{
    VContact *user = new VContact(buddy, account());
    if (ChatSession *session = ChatLayer::get(this, false))
        session->addContact(user);
    m_buddies.insert(buddy, user);
    connect(user, SIGNAL(destroyed(QObject*)), SLOT(onUserDestroyed(QObject*)));
}

void VGroupChat::onBuddyRemoved(vk::Buddy *buddy)
{
    VContact *user = m_buddies.take(buddy);
    if (ChatSession *session = ChatLayer::get(this, false))
        session->removeContact(user);

    if (ChatSession *session = ChatLayer::get(user, false)) {
        QObject::connect(session, SIGNAL(destroyed()), user, SLOT(deleteLater()));
    } else {
        m_buddies.remove(buddy);
        user->deleteLater();
    }
}

void VGroupChat::onUserDestroyed(QObject *obj)
{
    VContact *user = static_cast<VContact*>(obj);
    m_buddies.remove(m_buddies.key(user));
}

VAccount *VGroupChat::account() const
{
    return static_cast<VAccount*>(account());
}

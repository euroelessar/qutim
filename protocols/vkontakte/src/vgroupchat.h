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
#include "vaccount.h"

namespace vk {
class GroupChatSession;
class Buddy;
}

class VGroupChat : public qutim_sdk_0_3::Conference
{
public:
    VGroupChat(VAccount *account, int chatId);
    ~VGroupChat();
protected:
    virtual void doJoin();
    virtual void doLeave();
protected slots:
    void onBuddyAdded(vk::Buddy *buddy);
    void onBuddyRemoved(vk::Buddy *buddy);
    void onUserDestroyed(QObject *obj);
    void onJoinedChanged(bool set);
private:
    VAccount *account() const;
    vk::GroupChatSession *m_chatSession;
    QHash<vk::Buddy*, VContact*> m_buddies;
};

#endif // VGROUPCHAT_H

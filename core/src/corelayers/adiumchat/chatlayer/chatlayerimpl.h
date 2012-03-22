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

#ifndef CHATLAYERIMPL_H
#define CHATLAYERIMPL_H
#include <qutim/chatsession.h>
#include <QWeakPointer>
#include "chatlayerplugin.h"
#include "chatlayer_global.h"

using namespace qutim_sdk_0_3;

namespace Core
{
namespace AdiumChat
{

class ChatWidget;
class ChatSessionImpl;
class ConfTabCompletion;
typedef QHash<ChatUnit*,  ChatSessionImpl*> ChatSessionHash;
typedef QList<ChatSessionImpl*> ChatSessionList;
class ADIUMCHAT_EXPORT ChatLayerImpl : public ChatLayer
{
	Q_OBJECT
	Q_CLASSINFO("Uses", "IconLoader")
	Q_CLASSINFO("Uses", "MetaContactManager")
	Q_CLASSINFO("Uses", "ContactDelegate")
public:
	virtual QList<ChatSession* > sessions();
	ChatLayerImpl();
	virtual ~ChatLayerImpl();
	virtual ChatSession* getSession(ChatUnit* unit, bool create = true);
	static QIcon iconForState(ChatState state, const ChatUnit *unit = 0);
	static void insertText(ChatSessionImpl *session, const QString &text, bool setFocus = true);
	inline void onUnitChanged(ChatUnit *from, ChatUnit *to)
	{ m_chatSessions.insert(to, m_chatSessions.take(from)); }
private slots:
	void onChatSessionDestroyed(QObject *object);
	void onChatSessionActivated(bool activated);
	void onServiceChanged(QObject *now);
private:
	ChatSessionHash  m_chatSessions;
	QWeakPointer<ConfTabCompletion> m_tabCompletion;
};

}
}
#endif // CHATLAYERIMPL_H


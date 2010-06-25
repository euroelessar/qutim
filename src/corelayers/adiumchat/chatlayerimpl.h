/****************************************************************************
 *  chatlayerimpl.h
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
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

#ifndef CHATLAYERIMPL_H
#define CHATLAYERIMPL_H
#include <libqutim/messagesession.h>
#include <QPointer>

using namespace qutim_sdk_0_3;

namespace Core
{
	namespace AdiumChat
	{

		class ChatWidget;
		class ChatSessionImpl;
		class ConfTabCompletion;
		typedef QHash<ChatUnit *,  ChatSessionImpl *> ChatSessionHash;
		typedef QList<ChatSessionImpl *> ChatSessionList;
		class ChatLayerImpl : public ChatLayer
		{
			Q_OBJECT
			Q_CLASSINFO("Uses", "IconLoader")
			Q_CLASSINFO("Uses", "MetaContactManager")
		public:
			virtual QList<ChatSession* > sessions();
			ChatLayerImpl();
			virtual ~ChatLayerImpl();
			virtual ChatSession* getSession(ChatUnit* unit, bool create = true);
			static QIcon iconForState(ChatState state, const ChatUnit *unit = 0);
			inline void onUnitChanged(ChatUnit *from, ChatUnit *to) 
			{ m_chat_sessions.insert(to, m_chat_sessions.take(from)); }
		private slots:
			void onChatSessionDestroyed(QObject *object);
			void onChatSessionActivated(bool activated);
		private:
			ChatSessionHash  m_chat_sessions;
			QPointer<ConfTabCompletion> m_tab_completion;
		};
	}
}
#endif // CHATLAYERIMPL_H

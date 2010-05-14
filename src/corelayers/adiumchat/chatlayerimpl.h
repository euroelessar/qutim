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
		typedef QHash<ChatUnit *,  ChatSessionImpl *> ChatSessionHash;
		typedef QList<ChatSessionImpl *> ChatSessionList;
		class ChatLayerImpl : public ChatLayer
		{
			Q_OBJECT
			Q_CLASSINFO("Uses", "IconLoader")
		public:
			virtual QList<ChatSession* > sessions();
			ChatLayerImpl();
			virtual ~ChatLayerImpl();
			virtual ChatSession* getSession(ChatUnit* unit, bool create = true);
		private slots:
			void onChatSessionDestroyed(QObject *object);
		private:
			ChatSessionHash  m_chat_sessions;

		};

	}
}
#endif // CHATLAYERIMPL_H

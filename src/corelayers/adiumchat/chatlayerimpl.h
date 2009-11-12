/****************************************************************************
 *  chatlayerimpl.h
 *
 *  Copyright (c) 2009 by Sidorov Aleksey <sauron@citadelspb.com>
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

namespace AdiumChat
{

	class ChatWidget;
	class ChatSessionImpl;
	typedef QHash<QString,  ChatSessionImpl *> ChatSessionHash;
	typedef QList<ChatSessionImpl *> ChatSessionList;
	class ChatLayerImpl : public ChatLayer
	{
		Q_OBJECT
	public:
		virtual ChatSession* getSession ( Account* acc, const QString& id, bool create = true );
		virtual QList<ChatSession* > sessions();
		ChatLayerImpl();
		virtual ~ChatLayerImpl();
		virtual ChatSession* getSession(ChatUnit* unit, bool create = true);
	private slots:
		void onAccountDestroyed(QObject *object);
		void onSessionRemoved(Account *acc, const QString &id);
		void onChatWidgetDestroyed(QObject *object);
		void onSessionActivated(bool active);
	private:
		inline QString getWidgetId(Account* acc, const QString& id) const;
		inline QString getWidgetId(ChatSessionImpl *sess) const;
		//QHash<Account * , QPair<QPointer<Account>, ChatSessionHash> >  m_chat_sessions;//TODO more safe
		QHash<Account * , ChatSessionHash >  m_chat_sessions;
		QHash<QString, ChatWidget *> m_chatwidget_list;
	};

}
#endif // CHATLAYERIMPL_H

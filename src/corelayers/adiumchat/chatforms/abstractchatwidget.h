/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#ifndef ABSTRACTCHATWIDGET_H
#define ABSTRACTCHATWIDGET_H

#include <QMainWindow>
#include <QTimer>
#include <QTextCursor>
#include "../chatlayerimpl.h"

class QPlainTextEdit;
class QListView;

namespace Core
{
	namespace AdiumChat
	{
		enum ChatFlag
		{
			AeroThemeIntegration	=	0x01,
			ChatStateIconsOnTabs	=	0x02,
			SendTypingNotification	=	0x04,
			ShowUnreadMessages		=	0x08,
			SwitchDesktopOnRaise	=	0x10
		};
		Q_DECLARE_FLAGS(ChatFlags, ChatFlag);
		enum SendMessageKey
		{
			SendEnter = 0,
			SendCtrlEnter,
			SendDoubleEnter
		};
		
		class AbstractChatWidget : public QMainWindow
		{
			Q_OBJECT
		public:
			explicit AbstractChatWidget(const QString &key, bool removeSessionOnClose);
//			void clear();//remove all sessions
//			ChatSessionList getSessionList() const;
			virtual ~AbstractChatWidget();
			bool contains(ChatSessionImpl *session);
			virtual QPlainTextEdit *getInputField() = 0;
			virtual QTabBar *getTabBar() = 0;
			virtual QListView *getContactsView() = 0;
			virtual ChatSessionImpl *currentSession() = 0;
			virtual bool eventFilter(QObject *obj, QEvent *event);
		public slots:
			virtual void addSession(ChatSessionImpl *session) = 0;
			virtual void addSession(const ChatSessionList &sessions) = 0;
			virtual void removeSession(ChatSessionImpl *session) = 0;
			virtual void activate(AdiumChat::ChatSessionImpl* session) = 0;
		protected slots:
			void onSendButtonClicked();
		public slots:
			void raise();
		protected:
			virtual void onLoad();
			virtual void chatStateChanged(ChatState state, ChatSessionImpl *session) = 0;
			const QString m_key;
			const bool m_removeSessionOnClose;
			ChatSessionList m_sessions;
			ChatFlags m_chatFlags;
			bool m_htmlMessage;
			ChatState m_chatstate;
			SendMessageKey m_sendKey;
			int m_entersNumber;
			QTextCursor m_enterPosition;
			QTimer m_chatstateTimer;
		};
	}
}

#endif // ABSTRACTCHATWIDGET_H

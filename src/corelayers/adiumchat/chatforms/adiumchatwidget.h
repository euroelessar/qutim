/****************************************************************************
 *  adiumchatwidget.h
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

#ifndef ADIUMCHATWIDGET_H_H
#define ADIUMCHATWIDGET_H_H

#include <QWidget>
#include "../chatlayerimpl.h"
#include "abstractchatwidget.h"
#include <QMainWindow>
#include <QTimer>

namespace qutim_sdk_0_3 {
	class ActionToolBar;
}
class QPlainTextEdit;

namespace Ui
{
	class AdiumChatForm;
}
namespace Core
{
	namespace AdiumChat
	{
		class ChatSessionImpl;
		class ConfTabCompletion;
		class AdiumChatWidget : public AbstractChatWidget
		{
			Q_OBJECT
		public:
			AdiumChatWidget(const QString &key, bool removeSessionOnClose);
			void clear();//remove all sessions
			ChatSessionList getSessionList() const;
			virtual ~AdiumChatWidget();
			QPlainTextEdit *getInputField();
			QTabBar *getTabBar();
			QListView *getContactsView();
			ChatSessionImpl *currentSession();
			QWebView* getChatView();
		public slots:
			void addSession(ChatSessionImpl *session);
			void addSession(const ChatSessionList &sessions);
			void removeSession(ChatSessionImpl *session);
			void activate(AdiumChat::ChatSessionImpl* session);
			void onUnreadChanged(const qutim_sdk_0_3::MessageList &unread);
		protected:
			virtual bool event(QEvent *event);
			void loadSettings();
		private slots:
			void currentIndexChanged (int index);
			void onTabMoved(int from,int to);
			void onCloseRequested(int index);
			void onSessionDestroyed(QObject* object);
			void onTabContextMenu(const QPoint &pos);
			void onSessionListActionTriggered();
			void onShowHistory(); //TODO Move to startup module
			void onUnitTitleChanged(const QString &title);
		private:
			void chatStateChanged(ChatState state, ChatSessionImpl* session);
			int m_send_key;
			Ui::AdiumChatForm *ui;
			ActionToolBar *m_toolbar;
			QAction *m_session_list;
			QAction *m_reciever_selector;
		};
	}
}
#endif // ADIUMCHATWIDGET_H

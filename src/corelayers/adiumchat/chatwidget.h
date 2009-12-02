/****************************************************************************
 *  chatwidget.h
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

#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QWidget>
#include "chatlayerimpl.h"
#include <QMainWindow>

namespace qutim_sdk_0_3 {
	class ActionToolBar;
}

namespace Ui
{
	class AdiumChatForm;
}
namespace AdiumChat
{
	enum ChatFlag
	{
		RemoveSessionOnClose	=	0x1, //remove session, when widget or tab was closed
		AeroThemeIntegration	=	0x2,
		IconsOnTabs				=	0x4,
	};
	Q_DECLARE_FLAGS(ChatFlags, ChatFlag);
	class ChatSessionImpl;
	class ChatWidget : public QMainWindow
	{
		Q_OBJECT
	public:
		ChatWidget(ChatFlags chatFlags = RemoveSessionOnClose);
		void clear();//remove all sessions
		ChatSessionList getSessionList() const;
		virtual ~ChatWidget();
		bool contains (ChatSessionImpl *session);
	public slots:
		void addSession(ChatSessionImpl *session);
		void addSession(const ChatSessionList &sessions);
		void removeSession(ChatSessionImpl *session);
		void activate(ChatSessionImpl *session);
	protected:
		bool eventFilter(QObject *obj, QEvent *event);
	private:
		ChatSessionList m_sessions;
		Ui::AdiumChatForm *ui;
		ChatFlags m_chat_flags;
		QString m_chat_icon_type;
	private slots:
		void currentIndexChanged (int index);
		void onCloseRequested(int index);
		void onTabMoved(int from,int to);
		void onSessionRemoved();
		void onSendButtonClicked();
	};

}
#endif // CHATWIDGET_H

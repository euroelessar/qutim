/****************************************************************************
 *  chatsessionimpl_p.h
 *
 *  Copyright (c) 2010 by Aleksey Sidorov <sauron@citadelspb.com>
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

#ifndef CHATSESSIONIMPL_P_H
#define CHATSESSIONIMPL_P_H
#include <QObject>
#include <QPointer>
#include <QTimer>
#include <QDateTime>
#include <libqutim/message.h>
#include <libqutim/status.h>

class QMenu;
class QTextDocument;
namespace qutim_sdk_0_3 {
class Contact;
class ChatUnit;
}

class QWebPage;
namespace Core
{
	
	namespace AdiumChat
	{

class ChatSessionModel;

		using namespace qutim_sdk_0_3;

		class ChatStyleOutput;
		class ChatSessionImpl;

		class ChatSessionImplPrivate : public QObject
		{
			Q_OBJECT
			Q_DECLARE_PUBLIC(ChatSessionImpl)
		public:
			ChatSessionImplPrivate();
			virtual ~ChatSessionImplPrivate();
			void loadHistory();
			ChatStyleOutput *chat_style_output;
			void statusChanged(const Status &status,Contact *contact, bool silent = false);
			QPointer<QWebPage> web_page;
			QPointer<ChatUnit> chat_unit;
			QPointer<QTextDocument> input;
			QPointer<QMenu> menu;
			ChatSessionModel *model;
			//additional info and flags
			QString previous_sender; // null if outcoming
			bool skipOneMerge;
			bool active;
			bool store_service_messages;
			bool separator;
			QDateTime lastDate;
			int groupUntil;
			QTimer inactive_timer;
			MessageList unread;
			ChatState myself_chat_state;
			ChatSessionImpl *q_ptr;
			ChatState statusToState(Status::Type type);
		public slots:
			void onStatusChanged(qutim_sdk_0_3::Status);
			void onLinkClicked(const QUrl &url);
			void onActiveTimeout();
		};

	}
}

#endif // CHATSESSIONIMPL_P_H

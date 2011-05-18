/****************************************************************************
 *  chatsessionimpl_p.h
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

#ifndef CHATSESSIONIMPL_P_H
#define CHATSESSIONIMPL_P_H
#include <QObject>
#include <QPointer>
#include <QTimer>
#include <QDateTime>
#include <qutim/message.h>
#include <qutim/status.h>
#include <qutim/chatunit.h>

class QMenu;
class QTextDocument;
class QActionGroup;
namespace qutim_sdk_0_3 {
class Contact;
}

class QWebPage;
namespace Core
{
namespace AdiumChat
{

using namespace qutim_sdk_0_3;

class ChatSessionModel;
class ChatSessionImpl;
class ChatViewController;
class ChatSessionImplPrivate : public QObject
{
	Q_OBJECT
	Q_DECLARE_PUBLIC(ChatSessionImpl)
public:
	ChatSessionImplPrivate();
	virtual ~ChatSessionImplPrivate();
	void fillMenu(QMenu *menu, ChatUnit *unit, const ChatUnitList &lowerUnits, bool root = true);
	ChatViewController *getController() const;
	void ensureController() const;
	mutable QPointer<QObject> controller;
	QPointer<ChatUnit> chatUnit;
	QPointer<ChatUnit> current_unit; // the unit chosen by user as receiver
	QPointer<ChatUnit> last_active_unit; // the unit a last message was from
	QPointer<QTextDocument> input;
	QPointer<QMenu> menu;
	QPointer<QActionGroup> group;
	QPointer<ChatSessionModel> model;
	//additional info and flags
	bool active;
	bool sendToLastActiveResource;
	bool notificationsInActiveChat;
	QTimer inactive_timer;
	MessageList unread;
	ChatState myself_chat_state;
	ChatSessionImpl *q_ptr;
	//ChatState statusToState(Status::Type type);
public slots:
	void onStatusChanged(qutim_sdk_0_3::Status now,qutim_sdk_0_3::Status old, bool silent = false);
	void onActiveTimeout();
	void onResourceChosen(bool active);
	void onSendToLastActiveResourceActivated(bool active);
	void onLowerUnitAdded();
	void refillMenu();
};

}
}

#endif // CHATSESSIONIMPL_P_H

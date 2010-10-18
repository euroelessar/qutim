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
	void statusChanged(const Status &status,Contact *contact, bool silent = false);
	void fillMenu(QMenu *menu, ChatUnit *unit, const ChatUnitList &lowerUnits, bool root = true);
	ChatViewController *getController() const;
	void ensureController() const;
	mutable QPointer<QObject> controller;
	QPointer<ChatUnit> chat_unit;
	QPointer<ChatUnit> current_unit; // the unit chosen by user as receiver
	QPointer<ChatUnit> last_active_unit; // the unit a last message was from
	QPointer<QTextDocument> input;
	QPointer<QMenu> menu;
	QPointer<QActionGroup> group;
	ChatSessionModel *model;
	//additional info and flags
	bool active;
	bool sendToLastActiveResource;
	QTimer inactive_timer;
	MessageList unread;
	ChatState myself_chat_state;
	ChatSessionImpl *q_ptr;
	ChatState statusToState(Status::Type type);
	Status::Type lastStatusType;
	QString lastStatusText;
public slots:
	void onStatusChanged(qutim_sdk_0_3::Status);
	void onActiveTimeout();
	void onResourceChosen(bool active);
	void onSendToLastActiveResourceActivated(bool active);
	void onLowerUnitAdded();
	void refillMenu();
};

}
}

#endif // CHATSESSIONIMPL_P_H

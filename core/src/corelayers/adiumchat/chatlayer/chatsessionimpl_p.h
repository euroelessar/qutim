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

#ifndef CHATSESSIONIMPL_P_H
#define CHATSESSIONIMPL_P_H
#include <QObject>
#include <QWeakPointer>
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
	ChatViewController *getController();
	void ensureController();
	QWeakPointer<QObject> controller;
	QWeakPointer<ChatUnit> chatUnit;
	QWeakPointer<ChatUnit> current_unit; // the unit chosen by user as receiver
	QWeakPointer<ChatUnit> last_active_unit; // the unit a last message was from
	QWeakPointer<QTextDocument> input;
	QWeakPointer<QMenu> menu;
	QWeakPointer<QActionGroup> group;
	QWeakPointer<ChatSessionModel> model;
	//additional info and flags
	bool sendToLastActiveResource;
	mutable bool hasJavaScript;
	QTimer inactive_timer;
	MessageList unread;
	ChatState myselfChatState;
	ChatSessionImpl *q_ptr;
public slots:
	void onActiveTimeout();
	void onResourceChosen(bool active);
	void onSendToLastActiveResourceActivated(bool active);
	void onLowerUnitAdded();
	void refillMenu();
};

}
}

#endif // CHATSESSIONIMPL_P_H


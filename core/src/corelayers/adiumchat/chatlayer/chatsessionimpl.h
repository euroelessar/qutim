/****************************************************************************
*  chatsessionimpl.h
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

#ifndef CHATSESSIONIMPL_H
#define CHATSESSIONIMPL_H
#include <qutim/messagesession.h>
#include <QTextDocument>
#include <QPointer>
#include <QTimer>
#include "chatlayer_global.h"

class QAbstractItemModel;
class QWebPage;
class ChatViewController;

namespace qutim_sdk_0_3
{
LIBQUTIM_EXPORT QString convertTimeDate(const QString &mac_format, const QDateTime &datetime);
}

namespace Core
{
namespace AdiumChat
{

using namespace qutim_sdk_0_3;
class ChatStyleOutput;
class ChatSessionModel;
class ChatSessionImplPrivate;

class ADIUMCHAT_EXPORT ChatSessionImpl : public ChatSession
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(ChatSessionImpl)
public:
	ChatSessionImpl(ChatUnit *unit, ChatLayer *chat);
	virtual ~ChatSessionImpl();
	virtual void addContact ( Buddy* c );
	virtual qint64 doAppendMessage(Message &message);
	virtual void removeContact ( Buddy* c );
	QObject *getController() const;
	Account *getAccount() const;
	QString getId() const;
	ChatUnit *getUnit() const;
	ChatUnit *getCurrentUnit() const; // Returns unit chosen by user as receiver
	virtual QTextDocument *getInputField();
	virtual void markRead(quint64 id = Q_UINT64_C(0xffffffffffffffff));
	virtual MessageList unread() const;
	QAbstractItemModel *getModel() const;
	virtual void setActive(bool active = true);
	virtual void setChatUnit(ChatUnit* unit);
	virtual bool isActive();
	QMenu *menu();
	bool event(QEvent *);
	void setChatState(ChatState state);
	ChatState getChatState() const;
signals:
	void buddiesChanged();
	void chatUnitChanged(qutim_sdk_0_3::ChatUnit *);
public slots:
	QVariant evaluateJavaScript(const QString &scriptSource);
	void clearChat();
	QString quote();
private:
	QScopedPointer<ChatSessionImplPrivate> d_ptr;
};

}
}

#endif // CHATSESSIONIMPL_H

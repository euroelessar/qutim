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

#ifndef CHATSESSIONIMPL_H
#define CHATSESSIONIMPL_H

#include <QTextDocument>
#include <QDateTime>
#include <QTimer>
#include "chatlayer_global.h"
#include <qutim/chatsession.h>

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
	Q_PROPERTY(bool supportJavaScript READ isJavaScriptSupported NOTIFY javaScriptSupportChanged)
public:
	ChatSessionImpl(ChatUnit *unit, ChatLayer *chat);
	virtual ~ChatSessionImpl();
	virtual void addContact ( Buddy* c );
	virtual qint64 doAppendMessage(Message &message);
	virtual void removeContact ( Buddy* c );
	QObject *controller();
	QObject *controller() const;
	Account *getAccount() const;
	QString getId() const;
	ChatUnit *getUnit() const;
	ChatUnit *getCurrentUnit() const; // Returns unit chosen by user as receiver
	MessageList lastMessages() const;
	virtual QTextDocument *getInputField();
	virtual void markRead(quint64 id = Q_UINT64_C(0xffffffffffffffff));
	virtual MessageList unread() const;
	QAbstractItemModel *getModel() const;
	void doSetActive(bool active = true);
	void setChatUnit(ChatUnit* unit);
	QMenu *menu(bool showReceiverId);
	void setChatState(ChatState state);
	ChatState getChatState() const;
	bool isJavaScriptSupported() const;
signals:
	void buddiesChanged();
	void chatUnitChanged(qutim_sdk_0_3::ChatUnit *);
	void javaScriptSupportChanged(bool has);
	void controllerDestroyed(QObject *);
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


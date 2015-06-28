/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef APPLICATIONWINDOWPLUGIN_H
#define APPLICATIONWINDOWPLUGIN_H

#include <qutim/chatsession.h>
#include "chatmessagemodel.h"
#include <QUrl>

namespace QuickChat
{
class ChatChannelUsersModel;

class ChatChannel : public qutim_sdk_0_3::ChatSession
{
	Q_OBJECT
	Q_PROPERTY(qutim_sdk_0_3::ChatUnit* unit READ unit WRITE setChatUnit NOTIFY unitChanged)
	Q_PROPERTY(int unreadCount READ unreadCount NOTIFY unreadCountChanged)
	Q_PROPERTY(QObject *page READ page WRITE setPage NOTIFY pageChanged)
	Q_PROPERTY(QObject* model READ model CONSTANT)
	Q_PROPERTY(QObject* units READ units CONSTANT)
	Q_PROPERTY(bool supportJavaScript READ supportJavaScript CONSTANT)
public:
	ChatChannel(qutim_sdk_0_3::ChatUnit *unit);
	virtual ~ChatChannel();

	virtual qutim_sdk_0_3::ChatUnit *getUnit() const;
	virtual void setChatUnit(qutim_sdk_0_3::ChatUnit* unit);
	virtual QTextDocument *getInputField();
	virtual void markRead(quint64 id);
	virtual qutim_sdk_0_3::MessageList unread() const;
	int unreadCount() const;
	virtual void addContact(qutim_sdk_0_3::Buddy *c);
	virtual void removeContact(qutim_sdk_0_3::Buddy *c);
	QObject *model() const;
	Q_INVOKABLE void send(const QString &text);
	Q_INVOKABLE void showChat();
	Q_INVOKABLE void close();
	Q_INVOKABLE void clear();
	QObject *units() const;
	QObject *page() const;
	void setPage(QObject *page);
	bool supportJavaScript() const;

	QString id() const;

	bool event(QEvent *ev);

public slots:
	QVariant evaluateJavaScript(const QString &script);
	QString htmlEscape(const QString &text);
	void appendText(const QString &text);
	void appendNick(const QString &nick);
	void loadHistory();

	QUrl appendTextUrl(const QString &text);
	QUrl appendNickUrl(const QString &nick);

	int hashOf(const QString &nick, int max);

protected:
	QUrl commandUrl(const QString &method, const QString &arg) const;
	virtual qint64 doAppendMessage(qutim_sdk_0_3::Message &message);
	virtual void doSetActive(bool active);
	void connectNotify(const QMetaMethod &signal);
	void disconnectNotify(const QMetaMethod &signal);

signals:
	void javaScriptRequest(const QString &script);
	void messageAppended(const qutim_sdk_0_3::Message &message);
	void unitChanged(qutim_sdk_0_3::ChatUnit *unit);
	void unreadCountChanged(int);
	void pageChanged(QObject *page);
	void appendTextRequested(const QString &text);
	void appendNickRequested(const QString &nick);
	void receivedMessageReceipt(qint64 id, bool success);
	void clearRequested();

private:
	QString m_id;
	qutim_sdk_0_3::ChatUnit *m_unit;
	qutim_sdk_0_3::MessageList m_unread;
	ChatMessageModel *m_model;
	ChatChannelUsersModel *m_units;
	QObject *m_page;
	int m_javaScriptListeners = 0;
};
}

#endif // APPLICATIONWINDOWPLUGIN_H


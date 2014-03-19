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

#ifndef QUICKCHATVIEWCONTROLLER_H
#define QUICKCHATVIEWCONTROLLER_H

#include <qutim/adiumchat/chatviewfactory.h>
#include <qutim/chatsession.h>
#include <QQuickItem>
#include <QVariant>
#include <QPointer>

namespace Core {
namespace AdiumChat {

class QuickChatController : public QObject, public Core::AdiumChat::ChatViewController
{
    Q_OBJECT
	Q_INTERFACES(Core::AdiumChat::ChatViewController)
	Q_PROPERTY(QObject* session READ getSession NOTIFY sessionChanged)
	Q_PROPERTY(QObject* unit READ unit NOTIFY sessionChanged)
	Q_PROPERTY(QString chatState READ chatState NOTIFY chatStateChanged)
public:
    QuickChatController(QObject *parent = 0);
	virtual ~QuickChatController();
	virtual void setChatSession(qutim_sdk_0_3::ChatSession *session);
	virtual qutim_sdk_0_3::ChatSession *getSession() const;
	virtual void appendMessage(const qutim_sdk_0_3::Message &msg);
	virtual void clearChat();
	QQuickItem *rootItem() const;
	Q_INVOKABLE QString parseEmoticons(const QString &) const;
	QObject *unit() const;
	QString chatState() const;
public slots:
	void loadSettings();
	void loadTheme(const QString &name);
	void appendText(const QString &string);
protected slots:
	void loadHistory();
	void onChatStateChanged(qutim_sdk_0_3::ChatState state);
protected:
	bool eventFilter(QObject *, QEvent *);
signals:
	void messageAppended(const QVariant &message);
	void messageDelivered(int mid);
	void clearChatField();
	void sessionChanged(QObject *);
	void chatStateChanged(QString state);
private:
	QPointer<qutim_sdk_0_3::ChatSession> m_session;
    QPointer<QQuickItem> m_item;
	QString m_themeName;
};

} // namespace AdiumChat
} // namespace Core

Q_DECLARE_METATYPE(Core::AdiumChat::QuickChatController*)

#endif // QUICKCHATVIEWCONTROLLER_H


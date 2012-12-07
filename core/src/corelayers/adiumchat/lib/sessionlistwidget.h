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
#ifndef SESSIONLISTWIDGET_H
#define SESSIONLISTWIDGET_H

#include <QListWidget>
#include <qutim/chatsession.h>
#include <qutim/adiumchat/abstractchatwidget.h>

namespace Core {
namespace AdiumChat {

using namespace qutim_sdk_0_3;

class ChatSessionImpl;
class SessionListWidgetPrivate;
class ADIUMCHAT_EXPORT SessionListWidget : public QListWidget
{
    Q_OBJECT
	Q_DECLARE_PRIVATE(SessionListWidget)
public:
    explicit SessionListWidget(QWidget *parent = 0);
	void addSession(ChatSessionImpl *session);
	void removeSession(ChatSessionImpl *session);
	ChatSessionImpl *session(int index) const;
	void setCurrentSession(ChatSessionImpl *session);
	ChatSessionImpl *currentSession() const;
	bool contains(ChatSessionImpl *session) const;
	int indexOf(ChatSessionImpl *session) const;
	void removeItem(int index);
	virtual ~SessionListWidget();
public slots:
	void closeCurrentSession();
signals:
	void remove(ChatSessionImpl *session);
protected:
	virtual bool event(QEvent *event);
	void changeEvent(QEvent *ev);
	void chatStateChanged(ChatState state,ChatSessionImpl *session);
private slots:
	void onActivated(QListWidgetItem*);
	void onRemoveSession(QObject *obj);
	void onTitleChanged(const QString &title);
	void onUnreadChanged(const qutim_sdk_0_3::MessageList &unread);
	void onChatStateChanged(qutim_sdk_0_3::ChatState now, qutim_sdk_0_3::ChatState old);
	void onCloseSessionTriggered();
	void initScrolling();
private:
	QScopedPointer<SessionListWidgetPrivate> d_ptr;
};

} // namespace AdiumChat
} // namespace Core

#endif // SESSIONLISTWIDGET_H


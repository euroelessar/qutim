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
#ifndef TABBAR_H
#define TABBAR_H

#include <QTabBar>
#include <QMouseEvent>
#include <qutim/chatsession.h>
#include <qutim/chatunit.h>

namespace Core
{
namespace AdiumChat
{

using namespace qutim_sdk_0_3;

class ChatSessionImpl;
struct TabBarPrivate;
class TabBar : public QTabBar
{
	Q_OBJECT
public:
	TabBar(QWidget *parent = 0);
	~TabBar();
	void setClosableActiveTab(bool state);
	bool closableActiveTab();
	void setTabsClosable(bool closable);
	void addSession(ChatSessionImpl *session);
	void removeSession(ChatSessionImpl *session);
	ChatSessionImpl *session(int index) const;
	void setCurrentSession(ChatSessionImpl *session);
	ChatSessionImpl *currentSession() const;
	bool contains(ChatSessionImpl *session) const;
	int indexOf(ChatSessionImpl *session) const;
	void removeTab(int index);
	QMenu *menu() const; //ChatSessionlist
public slots:
	void showNextTab();
	void showPreviousTab();
	void closeCurrentTab();
	void onMoveTabLeftActivated();
	void onMoveTabRightActivated();
signals:
	void remove(ChatSessionImpl *session);
protected:
	void mouseMoveEvent(QMouseEvent *event);
	void leaveEvent(QEvent *event);
	virtual bool event(QEvent *event);
	void chatStateChanged(ChatState state, ChatSessionImpl *session);
	void statusChanged(const Status &status, ChatSessionImpl *session);
	void setSessionIcon(ChatSessionImpl *session, const QIcon &icon);
private slots:
	void onCurrentChanged(int index);
	void onCloseRequested(int index);
	void onRemoveSession(QObject *obj);
	void onTabMoved(int from,int to);
	void onTitleChanged(const QString &title);
	void onUnreadChanged(const qutim_sdk_0_3::MessageList &unread);
	void onContextMenu(const QPoint &pos);
	void onSessionListActionTriggered(QAction *);
	void onChatStateChanged(qutim_sdk_0_3::ChatState now, qutim_sdk_0_3::ChatState old);
	void onStatusChanged(const qutim_sdk_0_3::Status &status);
	void onCloseButtonClicked();
private:
	QScopedPointer<TabBarPrivate> p;
};

}
}

#endif //TABBAR_H


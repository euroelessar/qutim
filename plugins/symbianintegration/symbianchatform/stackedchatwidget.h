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
#ifndef STACKEDCHATWIDGET_H
#define STACKEDCHATWIDGET_H

#include <QWidget>
#include <qutim/adiumchat/abstractchatwidget.h>
#include <slidingstackedwidget.h>

namespace qutim_sdk_0_3
{
class ActionBox;
}

class QVBoxLayout;

namespace Core
{
namespace AdiumChat
{

class ChatViewWidget;
class SessionListWidget;
class ChatEdit;
class ConferenceContactsView;

namespace Symbian
{

using namespace qutim_sdk_0_3;

class StackedChatWidget : public AbstractChatWidget
{
    Q_OBJECT
public:
	StackedChatWidget(QWidget *parent = 0);
	virtual void addAction(ActionGenerator *gen);
	virtual QPlainTextEdit *getInputField() const;
	virtual bool contains(ChatSessionImpl *session) const;
	virtual ChatSessionImpl *currentSession()  const;
	virtual ~StackedChatWidget();
	void activateWindow();
	void addSessions(const ChatSessionList &sessions);
public slots:
	virtual void addSession(ChatSessionImpl *session);
	virtual void removeSession(ChatSessionImpl *session);
	virtual void activate(ChatSessionImpl* session);
	virtual void loadSettings();
protected:
	bool event(QEvent *event);
	void changeEvent(QEvent *ev);
	void setTitle(ChatSessionImpl *s);
	void onAboutToChangeIndex(int index); //Symbian softkeys workaround
protected slots:	
	void onSessionActivated(bool active);
	void onUnreadChanged();
	void onCurrentChanged(int index);
	void animationFinished();
	void fingerGesture(enum SlidingStackedWidget::SlideDirection);
	void onToggleFullscreen();
private:
	ChatViewWidget *m_view;
	SessionListWidget *m_sessionList;
	ChatEdit *m_chatInput;
	QPointer<ChatSessionImpl> m_currentSession;
	QAction *m_receiverList;
	SlidingStackedWidget *m_stack;
	QWidget *m_chatWidget;
	QPointer<QAction> m_unitActions;
	QAction *m_sendAct;
	ConferenceContactsView *m_confContactView;
	QWidget *m_contactList;
	MenuController *m_menu;
};

} // namespace Symbian
} // namespace AdiumChat
} // namespace Core

#endif // STACKEDCHATWIDGET_H


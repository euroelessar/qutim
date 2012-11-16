/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Evgeniy Degtyarev <degtep@gmail.com>
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

#ifndef STACKEDCHATWIDGET_H
#define STACKEDCHATWIDGET_H

#include <QWidget>
#include <qutim/adiumchat/abstractchatwidget.h>
#include <slidingstackedwidget.h>
#include <qutim/adiumchat/chatsessionimpl.h>
#include <qutim/adiumchat/chatlayerimpl.h>
#include "floatingbutton.h"

#ifdef Q_WS_MAEMO_5
#include <kb_qwerty.h>
#endif


namespace qutim_sdk_0_3
{
	class ActionToolBar;
}

class SlidingStackedWidget;
class QVBoxLayout;

namespace Core
{
namespace AdiumChat
{

using namespace qutim_sdk_0_3;


enum ChatFlag
{
	SendTypingNotification	=       0x01,
	SwitchDesktopOnActivate	=       0x02,
	DeleteSessionOnClose	=	0x04,
};

Q_DECLARE_FLAGS(ChatFlags, ChatFlag);

class ChatViewWidget;
class SessionListWidget;
class ChatEdit;
class ConferenceContactsView;
class StackedChatWidget : public AbstractChatWidget
{
    Q_OBJECT
public:
	StackedChatWidget(const QString &key = QString(),QWidget *parent = 0);
	virtual void addAction(ActionGenerator *gen);
	virtual QTextEdit *getInputField() const;
	virtual bool contains(ChatSessionImpl *session) const;
	virtual ChatSessionImpl *currentSession()  const;
	virtual ~StackedChatWidget();
	void activateWindow();
	void addSessions(const ChatSessionList &sessions);
	Q_INVOKABLE ActionToolBar *toolBar() const;
public slots:
	virtual void addSession(ChatSessionImpl *session);
	virtual void removeSession(ChatSessionImpl *session);
	virtual void activate(ChatSessionImpl* session);
	virtual void loadSettings();
protected:
	bool event(QEvent *event);
protected slots:	
	void onSessionActivated(bool active);
	void onUnreadChanged();
	void onCurrentChanged(int index);
	void animationFinished();
	void fingerGesture(enum SlidingStackedWidget::SlideDirection);
#ifdef Q_WS_MAEMO_5
	void processInput(QString);
	void orientationChanged();
	void showKeyboard();
#endif
	void showContactList();
private:
	ActionToolBar *m_toolbar;
	ChatViewWidget *m_view;
	SessionListWidget *m_sessionList;
	ChatEdit *m_chatInput;
	QWeakPointer<ChatSessionImpl> m_currentSession;
	QAction *m_unitSeparator;
	QAction *m_actSeparator;
	QAction *m_spacer;
	QAction *m_receiverList;
	QAction *m_unitActions;
	ConferenceContactsView *m_contactView;
	QString m_key;
	SlidingStackedWidget *m_stack;
	ChatFlags m_flags;
	QWidget *m_chatWidget;
	QToolBar *m_additionalToolBar;
	QMenuBar *menuBar;
#ifdef Q_WS_MAEMO_5
	FloatingButton *showKeyb;
	bool isPortraitMode;
	kb_Qwerty *m_kb_qwerty;
#endif
};

}
}

Q_DECLARE_OPERATORS_FOR_FLAGS(Core::AdiumChat::ChatFlags)

#endif // STACKEDCHATWIDGET_H


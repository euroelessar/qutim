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
#ifndef TABBEDCHATWIDGET_H
#define TABBEDCHATWIDGET_H

#include <QWidget>
#include <chatforms/abstractchatwidget.h>


namespace qutim_sdk_0_3
{
	class ActionToolBar;
}
class QVBoxLayout;
class QSplitter;
namespace Core
{
namespace AdiumChat
{

using namespace qutim_sdk_0_3;


enum ChatFlag
{
	AdiumToolbar			=       0x01,
	IconsOnTabs				=       0x02,
	SendTypingNotification	=       0x04,
	SwitchDesktopOnActivate	=       0x08,
	DeleteSessionOnClose	=		0x10,
	TabsOnBottom			=		0x20,
	MenuBar					=		0x40,
	UseQutimIcon            =       0x80
};

Q_DECLARE_FLAGS(ChatFlags, ChatFlag)

class ChatViewWidget;
class TabBar;
class ChatEdit;
class ConferenceContactsView;
class TabbedChatWidget : public AbstractChatWidget, public ActionHandler
{
    Q_OBJECT
public:
	TabbedChatWidget(const QString &key = QString(),QWidget *parent = 0);
	virtual void actionAdded(QAction *action, int index);
	virtual void actionRemoved(int index);
	virtual void actionsCleared();
	virtual void addAction(ActionGenerator *gen);
	virtual QTextEdit *getInputField() const;
	virtual bool contains(ChatSessionImpl *session) const;
	virtual ChatSessionImpl *currentSession()  const;
	virtual ~TabbedChatWidget();
	void setUnifiedTitleAndToolBar(bool set);
	void activateWindow();
	void addSessions(const ChatSessionList &sessions);
	Q_INVOKABLE ActionToolBar *toolBar() const;
	virtual void setView(QWidget *view);
public slots:
	virtual void addSession(ChatSessionImpl *session);
	virtual void removeSession(ChatSessionImpl *session);
	virtual void activate(ChatSessionImpl* session);
	virtual void loadSettings();
protected:
	bool event(QEvent *event);
	bool eventFilter(QObject *obj, QEvent *event);
protected slots:	
	void onSessionActivated(bool active);
	void ensureToolBar();
	void onUnreadChanged();
	void onControllerDestroyed(QObject *controller);
private:
	ActionToolBar *m_toolbar;
	ActionContainer m_actions;
	ChatViewWidget *m_chatViewWidget;
	TabBar *m_tabBar;
	ChatEdit *m_chatInput;
	QWeakPointer<ChatSessionImpl> m_currentSession;
	QAction *m_unitSeparator;
	QAction *m_actSeparator;
	QList<QAction*> m_unitChatActionList;
	QAction *m_spacer;
	QAction *m_receiverList;
	ConferenceContactsView *m_contactView;
	QString m_key;
	QVBoxLayout *m_layout;
	ChatFlags m_flags;
	QAction *m_sessionList;
	QAction *m_unitAction;
	QSplitter *m_vSplitter;
	QWidget *m_view;
};

}
}

Q_DECLARE_OPERATORS_FOR_FLAGS(Core::AdiumChat::ChatFlags)

#endif // TABBEDCHATWIDGET_H


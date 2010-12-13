#ifndef STACKEDCHATWIDGET_H
#define STACKEDCHATWIDGET_H

#include <QWidget>
#include <chatforms/abstractchatwidget.h>


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
	DeleteSessionOnClose	=		0x04,
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
	virtual QPlainTextEdit *getInputField() const;
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
private:
	ActionToolBar *m_toolbar;
	ChatViewWidget *m_view;
	SessionListWidget *m_sessionList;
	ChatEdit *m_chatInput;
	QPointer<ChatSessionImpl> m_currentSession;
	QAction *m_unitSeparator;
	QAction *m_actSeparator;
	QAction *m_spacer;
	QAction *m_recieverList;
	ConferenceContactsView *m_contactView;
	QString m_key;
	SlidingStackedWidget *m_stack;
	ChatFlags m_flags;
	QWidget *m_chatWidget;
	QAction *m_unitActions;
	QToolBar *m_additionalToolBar;
	QMenuBar *menuBar;
	Qt::GestureType fingerSwipeGestureType;
};

}
}

Q_DECLARE_OPERATORS_FOR_FLAGS(Core::AdiumChat::ChatFlags)

#endif // STACKEDCHATWIDGET_H

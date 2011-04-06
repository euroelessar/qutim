#ifndef STACKEDCHATWIDGET_H
#define STACKEDCHATWIDGET_H

#include <QWidget>
#include <chatforms/abstractchatwidget.h>
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
	QAction *m_recieverList;
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

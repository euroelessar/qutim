#ifndef TABBEDCHATWIDGET_H
#define TABBEDCHATWIDGET_H

#include <QWidget>
#include <chatlayer/chatforms/abstractchatwidget.h>

namespace qutim_sdk_0_3
{
	class ActionToolBar;
}

namespace Core
{
namespace AdiumChat
{

using namespace qutim_sdk_0_3;

class ChatViewWidget;
class TabBar;
class ChatEdit;
class ConferenceContactsView;
class TabbedChatWidget : public AbstractChatWidget
{
    Q_OBJECT
public:
	TabbedChatWidget(const QString &key = QString(),QWidget *parent = 0);
	virtual void addAction(ActionGenerator *gen);
	virtual QPlainTextEdit *getInputField() const;
	virtual bool contains(ChatSessionImpl *session) const;
	virtual ChatSessionImpl *currentSession()  const;
	virtual ~TabbedChatWidget();
public slots:
	virtual void addSession(ChatSessionImpl *session);
	void addSessions(const ChatSessionList &sessions);
	virtual void removeSession(ChatSessionImpl *session);
	virtual void activate(ChatSessionImpl* session);
	virtual void loadSettings();
protected:
	bool event(QEvent *event);
protected slots:	
	void onSessionActivated(bool active);
private:
	ActionToolBar *m_toolbar;
	ChatViewWidget *m_view;
	TabBar *m_tabbar;
	ChatEdit *m_chatInput;
	QPointer<ChatSessionImpl> m_currentSession;
	QAction *m_unitSeparator;
	QAction *m_actSeparator;
	QList<QAction*> m_unitActions;
	QAction *m_spacer;
	QAction *m_recieverList;
	ConferenceContactsView *m_contactView;
	QString m_key;
};

}
}
#endif // TABBEDCHATWIDGET_H

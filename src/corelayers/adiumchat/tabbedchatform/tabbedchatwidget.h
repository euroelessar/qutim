#ifndef TABBEDCHATWIDGET_H
#define TABBEDCHATWIDGET_H

#include <QWidget>
#include <chatforms/abstractchatwidget.h>


namespace qutim_sdk_0_3
{
	class ActionToolBar;
}
class QVBoxLayout;
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
	TabsOnBottom			=		0x20
};

Q_DECLARE_FLAGS(ChatFlags, ChatFlag);

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
	void setUnifiedTitleAndToolBar(bool set);
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
	void ensureToolBar();
	void onUnreadChanged();
private:
	ActionToolBar *m_toolbar;
	ChatViewWidget *m_view;
	TabBar *m_tabBar;
	ChatEdit *m_chatInput;
	QPointer<ChatSessionImpl> m_currentSession;
	QAction *m_unitSeparator;
	QAction *m_actSeparator;
	QList<QAction*> m_unitActions;
	QAction *m_spacer;
	QAction *m_recieverList;
	ConferenceContactsView *m_contactView;
	QString m_key;
	QVBoxLayout *m_layout;
	ChatFlags m_flags;
	QAction *m_sessionList;
};

}
}

Q_DECLARE_OPERATORS_FOR_FLAGS(Core::AdiumChat::ChatFlags)

#endif // TABBEDCHATWIDGET_H

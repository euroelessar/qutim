#ifndef SESSIONLISTWIDGET_H
#define SESSIONLISTWIDGET_H

#include <QListWidget>
#include <qutim/messagesession.h>
#include <chatforms/abstractchatwidget.h>

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

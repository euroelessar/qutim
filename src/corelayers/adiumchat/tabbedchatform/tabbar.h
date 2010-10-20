#ifndef TABBAR_H
#define TABBAR_H

#include <QTabBar>
#include <QMouseEvent>
#include <qutim/messagesession.h>

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
	signals:
		void remove(ChatSessionImpl *session);
	protected:
		void mouseMoveEvent(QMouseEvent *event);
		void leaveEvent(QEvent *event);
		virtual bool eventFilter(QObject *obj, QEvent *event);
		virtual bool event(QEvent *event);
		void chatStateChanged(ChatState state,ChatSessionImpl *session);
	private slots:
		void onCurrentChanged(int index);
		void onCloseRequested(int index);
		void removeSession(QObject *obj);
		void onTabMoved(int from,int to);
		void onTitleChanged(const QString &title);
		void onUnreadChanged(const qutim_sdk_0_3::MessageList &unread);
		void onContextMenu(const QPoint &pos);
		void onSessionListActionTriggered(QAction *);
	private:
		QScopedPointer<TabBarPrivate> p;
};

}
}

#endif //TABBAR_H

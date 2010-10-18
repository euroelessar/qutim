#ifndef TABBAR_H
#define TABBAR_H

#include <QTabBar>
#include <QMouseEvent>

namespace Core
{
namespace AdiumChat
{

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
	signals:
		void activate(ChatSessionImpl *session);
		void remove(ChatSessionImpl *session);
	protected:
		void mouseMoveEvent(QMouseEvent *event);
		void leaveEvent(QEvent *event);
	private slots:
		void onCurrentChanged(int index);
		void onCloseRequested(int index);
		void onSessionDestroyed(QObject *obj);
		void onTabMoved(int from,int to);
		void onTitleChanged(const QString &title);
	private:
		QScopedPointer<TabBarPrivate> p;
};

}
}

#endif //TABBAR_H

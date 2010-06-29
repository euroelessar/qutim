#ifndef TABBAR_H
#define TABBAR_H

#include <QTabBar>
#include <QMouseEvent>

struct TabBarPrivate;

class TabBar : public QTabBar
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(TabBar)
	public:
		TabBar(QWidget *parent = 0);
		~TabBar();
		void setClosableActiveTab(bool state);
		bool closableActiveTab();
		void setTabsClosable(bool closable);
	protected:
		void mouseMoveEvent(QMouseEvent *event);
		void leaveEvent(QEvent *event);
	private:
		QScopedPointer<TabBarPrivate> d_ptr;
};

#endif //TABBAR_H

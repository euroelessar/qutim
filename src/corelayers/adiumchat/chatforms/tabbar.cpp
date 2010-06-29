#include "tabbar.h"
#include <QStyle>

struct TabBarPrivate
{
	bool closableActiveTab;
};

TabBar::TabBar(QWidget *parent) : QTabBar(parent), d_ptr(new TabBarPrivate())
{
	d_func()->closableActiveTab = false;
	setMouseTracking(true);
 }

TabBar::~TabBar()
{ 
}

void TabBar::setClosableActiveTab(bool state)
{
	d_func()->closableActiveTab = state;
	setTabsClosable(tabsClosable() | state);
} 

bool TabBar::closableActiveTab()
{
	return d_func()->closableActiveTab;
}

void TabBar::setTabsClosable(bool closable)
{
	d_func()->closableActiveTab = closable & d_func()->closableActiveTab;
	QTabBar::setTabsClosable(closable);
}

void TabBar::mouseMoveEvent(QMouseEvent *event)
{
	int hoveredTab = -1;
	if (d_func()->closableActiveTab)
		for (int tab = 0; tab < count(); tab++) {
			QTabBar::ButtonPosition closeSide = (QTabBar::ButtonPosition)style()->styleHint(QStyle::SH_TabBar_CloseButtonPosition, 0, this);
			if (QWidget *button = tabButton(tab, closeSide))
				button->setVisible(false);
			if (tabRect(tab).contains(event->pos()))
				hoveredTab = tab;
		}
	if (hoveredTab != -1) {
		QTabBar::ButtonPosition closeSide = (QTabBar::ButtonPosition)style()->styleHint(QStyle::SH_TabBar_CloseButtonPosition, 0, this);
		if (QWidget *button = tabButton(hoveredTab, closeSide))
			button->setVisible(true);
	}
	return QTabBar::mouseMoveEvent(event);
}

void TabBar::leaveEvent(QEvent *event)
{
	if (d_func()->closableActiveTab)
		for (int tab = 0; tab < count(); tab++) {
			QTabBar::ButtonPosition closeSide = (QTabBar::ButtonPosition)style()->styleHint(QStyle::SH_TabBar_CloseButtonPosition, 0, this);
			if (QWidget *button = tabButton(tab, closeSide))
				button->setVisible(false);
		}
	return QTabBar::leaveEvent(event);
}

#include "tabbar.h"
#include <QStyle>
#include <chatlayer/chatsessionimpl.h>
#include <chatlayer/chatlayerimpl.h>

namespace Core
{
namespace AdiumChat
{

using namespace qutim_sdk_0_3;

struct TabBarPrivate
{
	bool closableActiveTab;
	ChatSessionList sessions;
};

TabBar::TabBar(QWidget *parent) : QTabBar(parent), p(new TabBarPrivate())
{
	p->closableActiveTab = false;
	setMouseTracking(true);
	connect(this,SIGNAL(currentChanged(int)),SLOT(onCurrentChanged(int)));
	connect(this,SIGNAL(tabCloseRequested(int)),SLOT(onCloseRequested(int)));
	connect(this,SIGNAL(tabMoved(int,int)),SLOT(onTabMoved(int,int)));
 }

TabBar::~TabBar()
{
	foreach(ChatSessionImpl *s,p->sessions)
		removeSession(s);
}

void TabBar::setClosableActiveTab(bool state)
{
	p->closableActiveTab = state;
	setTabsClosable(tabsClosable() | state);
} 

bool TabBar::closableActiveTab()
{
	return p->closableActiveTab;
}

void TabBar::setTabsClosable(bool closable)
{
	p->closableActiveTab = closable & p->closableActiveTab;
	QTabBar::setTabsClosable(closable);
}

void TabBar::mouseMoveEvent(QMouseEvent *event)
{
	int hoveredTab = -1;
	if (p->closableActiveTab)
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
	if (p->closableActiveTab)
		for (int tab = 0; tab < count(); tab++) {
			QTabBar::ButtonPosition closeSide = (QTabBar::ButtonPosition)style()->styleHint(QStyle::SH_TabBar_CloseButtonPosition, 0, this);
			if (QWidget *button = tabButton(tab, closeSide))
				button->setVisible(false);
		}
	return QTabBar::leaveEvent(event);
}

void TabBar::addSession(ChatSessionImpl *session)
{
	p->sessions.append(session);
	addTab(session->getUnit()->title());

	connect(session->getUnit(),SIGNAL(titleChanged(QString,QString)),SLOT(onTitleChanged(QString)));
	connect(session,SIGNAL(destroyed(QObject*)),SLOT(onSessionDestroyed(QObject*)));
}

void TabBar::removeSession(ChatSessionImpl *session)
{
	removeTab(p->sessions.indexOf(session));
}

ChatSessionImpl *TabBar::session(int index) const
{
	if((index == -1) || (index >= p->sessions.count()))
		return 0;
	return p->sessions.at(index);
}

ChatSessionImpl *TabBar::currentSession() const
{
	return p->sessions.at(currentIndex());
}

void TabBar::setCurrentSession(ChatSessionImpl *session)
{
	setCurrentIndex(p->sessions.indexOf(session));
}

bool TabBar::contains(ChatSessionImpl *session) const
{
	return p->sessions.contains(session);
}

int TabBar::indexOf(ChatSessionImpl *session) const
{
	return p->sessions.indexOf(session);
}

void TabBar::onCurrentChanged(int index)
{
	if(ChatSessionImpl *s = session(index))
		emit activate(s);
}

void TabBar::onCloseRequested(int index)
{
	removeTab(index);
}

void TabBar::removeTab(int index)
{
	ChatSessionImpl *s = p->sessions.takeAt(index);
	s->disconnect(this);
	QTabBar::removeTab(index);
	emit remove(s);
}

void TabBar::onSessionDestroyed(QObject *obj)
{
	ChatSessionImpl *s = reinterpret_cast<ChatSessionImpl*>(obj);
	int index = p->sessions.indexOf(s);
	p->sessions.removeAll(s);
	QTabBar::removeTab(index);
}

void TabBar::onTabMoved(int from, int to)
{
	p->sessions.move(from,to);
}

void TabBar::onTitleChanged(const QString &title)
{
	ChatUnit *u = qobject_cast<ChatUnit*>(sender());
	ChatSessionImpl *s = static_cast<ChatSessionImpl*>(ChatLayer::get(u,false));
	setTabText(indexOf(s),title);
}

}
}

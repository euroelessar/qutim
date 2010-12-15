#include "TaskbarPreviews.h"
#include "WinEventFilter.h"
#include <QTimer>
#include <QApplication>
#include "../../apilayer/src/ApiTaskbarPreviews.h"

///////////////////////////////////////////////////////////////////////////////
// Functions, related to TaskbarPreviews class itself

TaskbarPreviews::TaskbarPreviews()
{
	void *p = Win7EventFilter::instance(); // initializing filter
	Q_UNUSED(p)
}

void TaskbarPreviews::addTab(QWidget *tab, QWidget *owner, const QString &title, QWidget *before, PreviewProvider *pp)
{
	_tab t = {0};
	t.internalTabWidget = new QWidget;
	t.pp            = pp;
	t.userTabWidget = tab;
	t.tabHandle     = t.internalTabWidget->winId();
	t.userTabOwner  = owner;
	if(title.length())
		t.internalTabWidget->setWindowTitle(title);
	else
		t.internalTabWidget->setWindowTitle(t.userTabOwner->windowTitle());
	HWND afterHwnd = before ? static_cast<HWND>(m_tabs.internal(before)->winId()) : 0; // will return zero if after == 0 or there's no such tab, which means that tab will be added to the end
	if (tab->window() == tab) {
		if(pp){
			ForceIconicRepresentation(t.internalTabWidget->winId());
		}
	} else // tab is not a window so previews must be created by library or user anyway
		ForceIconicRepresentation(t.internalTabWidget->winId());
	m_tabs.append(t);
	RegisterTab(t.tabHandle, owner->winId());
	SetTabOrder(t.tabHandle, afterHwnd);
}

void TaskbarPreviews::removeTab(QWidget *userWidgetTab)
{
	int i = 0;
	while(i < m_tabs.size()){
		_tab t = m_tabs.at(i);
		if(t.userTabWidget == userWidgetTab){
			UnregisterTab(t.internalTabWidget->winId());
			t.internalTabWidget->deleteLater();
			m_tabs.removeAt(i);
		}else
			i++;
	}
}

void TaskbarPreviews::activateTab(QWidget *tab)
{
	QWidget *internal = m_tabs.internal(tab);
	if(internal)
		SetTabActive(internal->winId(), m_tabs.owner(internal->winId())->winId());
}

void TaskbarPreviews::changeOrder(QWidget *tab, QWidget *before)
{
	QWidget *inttab   = m_tabs.internal(tab);
	QWidget *intbefore = m_tabs.internal(before);
	if(!(inttab && intbefore))
		return;
	SetTabOrder(inttab->winId(), intbefore->winId());
}

void TaskbarPreviews::widgetDestroyed()
{
	QWidget *w = qobject_cast<QWidget *>(sender());
	if(w)
		removeTab(w);
}

TaskbarPreviews *TaskbarPreviews::instance()
{
	static TaskbarPreviews _instance;
	return &_instance;
}

void TaskbarPreviews::clear()
{
	while(!this->m_tabs.empty())
		removeTab(m_tabs.first().userTabWidget);
}

void TaskbarPreviews::refreshPreviews(QWidget *tab)
{
	QWidget *w = m_tabs.internal(tab);
	if(w)
		InvalidateBitmaps(w->winId());
}

// Static functions

void TaskbarPreviews::tabAdd(QWidget *tab, QWidget *owner, const QString &title, QWidget *before, PreviewProvider *pp)
{
	TaskbarPreviews::instance()->addTab(tab, owner, title, before, pp);
}

void TaskbarPreviews::tabActivate(QWidget *tab)
{
	TaskbarPreviews::instance()->activateTab(tab);
}

void TaskbarPreviews::tabRemove(QWidget *tab)
{
	TaskbarPreviews::instance()->removeTab(tab);
}

void TaskbarPreviews::tabOrderChange(QWidget *tab, QWidget *before)
{
	TaskbarPreviews::instance()->changeOrder(tab, before);
}

void TaskbarPreviews::tabsClear()
{
	TaskbarPreviews::instance()->clear();
}

void TaskbarPreviews::tabPreviewsRefresh(QWidget *tab)
{
	TaskbarPreviews::instance()->refreshPreviews(tab);
}

// Functions, used by messages filter.

QPixmap TaskbarPreviews::IconicThumbnail(HWND hwnd, QSize size)
{
	QPixmap pixmap;
	QWidget *w = m_tabs.user(hwnd);
	PreviewProvider *pp = m_tabs.previews(hwnd);
	if(w){
		if(pp)
			pixmap = pp->IconicPreview(w, m_tabs.owner(hwnd), size);
		else
			pixmap = QPixmap::grabWidget(w).scaled(size, Qt::KeepAspectRatio);
	}
	return pixmap;
}

QPixmap TaskbarPreviews::IconicLivePreviewBitmap(HWND hwnd)
{
	QPixmap pixmap;
	QWidget *w = m_tabs.user(hwnd);
	PreviewProvider *pp = m_tabs.previews(hwnd);
	if(w){
		if(pp)
			pixmap = pp->LivePreview(w, m_tabs.owner(hwnd));
		else
			pixmap = QPixmap::grabWidget(w->window());
	}
	return pixmap;
}

bool TaskbarPreviews::WasTabActivated(HWND hwnd)
{
	QWidget *internal = m_tabs.internal(hwnd);
	if(internal){
		QWidget *owner = m_tabs.owner(hwnd);
		SetTabActive(internal->winId(), owner->winId());
		if(owner->isMinimized())
			owner->showNormal();
		qApp->setActiveWindow(owner);
		emit tabActivated(m_tabs.user(hwnd));
		return true;
	}else
		return false;
}

bool TaskbarPreviews::WasTabRemoved(HWND hwnd)
{
	QWidget *internal = m_tabs.internal(hwnd);
	if(internal){
		QWidget *owner = m_tabs.owner(hwnd);
		bool ignore = false;
		emit tabAboutToRemove(m_tabs.user(hwnd), &ignore);
		if(!ignore){
			SetNoTabActive(owner->winId());
			removeTab(m_tabs.user(hwnd));
		}
		return true;
	}else
		return false;
}

///////////////////////////////////////////////////////////////////////////////
// Functions of TabsList --­ tabs info container.

QWidget *TaskbarPreviews::TabsList::internal(HWND internalHandle)
{
	foreach(const TaskbarPreviews::_tab &t, *this){
		if(static_cast<void *>(t.internalTabWidget->winId()) == static_cast<void *>(internalHandle))
			return t.internalTabWidget;
	}
	return 0;
}

QWidget *TaskbarPreviews::TabsList::internal(QWidget *userWidget)
{
	foreach(const TaskbarPreviews::_tab &t, *this){
		if(t.userTabWidget == userWidget)
			return t.internalTabWidget;
	}
	return 0;
}

QWidget *TaskbarPreviews::TabsList::user(HWND internalHandle)
{
	foreach(const TaskbarPreviews::_tab &t, *this){
		if(static_cast<void *>(t.internalTabWidget->winId()) == static_cast<void *>(internalHandle))
			return t.userTabWidget;
	}
	return 0;
}

QWidget *TaskbarPreviews::TabsList::owner(HWND internalHandle)
{
	foreach(const TaskbarPreviews::_tab &t, *this){
		if(static_cast<void *>(t.internalTabWidget->winId()) == static_cast<void *>(internalHandle))
			return t.userTabOwner;
	}
	return 0;
}

void TaskbarPreviews::TabsList::insert(QWidget *internal, QWidget *userTab, QWidget *tabOwner, PreviewProvider *pp)
{
	TaskbarPreviews::_tab t;
	connect(userTab, SIGNAL(destroyed()), TaskbarPreviews::instance(), SLOT(widgetDestroyed()));
	t.internalTabWidget = internal;
	t.userTabWidget   = userTab;
	t.userTabOwner    = tabOwner;
	t.tabHandle   = static_cast<HWND>(internal->winId());
	t.pp          = pp;
	QList<_tab>::insert(0, t);
}

PreviewProvider *TaskbarPreviews::TabsList::previews(HWND internalHandle)
{
	foreach(const TaskbarPreviews::_tab &t, *this){
		if(static_cast<void *>(t.internalTabWidget->winId()) == static_cast<void *>(internalHandle))
			return t.pp;
	}
	return 0;
}

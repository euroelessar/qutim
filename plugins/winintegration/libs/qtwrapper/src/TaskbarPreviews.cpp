/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ivan Vizir <define-true-false@yandex.com>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#include "TaskbarPreviews.h"
#include "WinEventFilter.h"
#include <QTimer>
#include <QApplication>
#include "../../apilayer/src/ApiTaskbarPreviews.h"

TaskbarPreviews::_tab::_tab()
{
	memset(this, 0, sizeof(_tab));
	static unsigned nextid = 0;
	this->id = ++nextid;
}

///////////////////////////////////////////////////////////////////////////////
// Functions, related to TaskbarPreviews class itself

TaskbarPreviews::TaskbarPreviews()
{
	Win7EventFilter::instance(); // initializing filter
}

unsigned TaskbarPreviews::addTab(QWidget *tab, QWidget *owner, const QString &title, QWidget *before, PreviewProvider *pp)
{
	_tab t;
	t.internalTabWidget = new QWidget;
	t.pp            = pp;
	t.userTabWidget = tab;
	t.tabHandle     = (HWND)t.internalTabWidget->winId();
	t.userTabOwner  = owner;
	t.type          = (tab->window() == tab) ? ttWindow : ttWidget;
	if (!title.isNull())
		t.internalTabWidget->setWindowTitle(title);
	else
		t.internalTabWidget->setWindowTitle(t.userTabOwner->windowTitle());
	HWND afterHwnd = before ? reinterpret_cast<HWND>(m_tabs.internal(before)->winId()) : 0; // will return zero if after == 0 or there's no such tab, in that cases tab will be added to the end
	if (tab->window() == tab) {
		if(pp){
			ForceIconicRepresentation((HWND)t.internalTabWidget->winId());
		}
	} else // tab is not a window so previews must be created by library or user anyway
		ForceIconicRepresentation((HWND)t.internalTabWidget->winId());
	m_tabs.append(t);
	RegisterTab(t.tabHandle, (HWND)owner->winId());
	SetTabOrder(t.tabHandle, afterHwnd);
	return t.id;
}

unsigned TaskbarPreviews::addVirtualTab(PreviewProvider *pp, QWidget *owner, const QString &title, QWidget *before)
{
	_tab t;
	t.internalTabWidget = new QWidget;
	t.pp            = pp;
	t.userTabWidget = 0;
	t.tabHandle     = (HWND)t.internalTabWidget->winId();
	t.userTabOwner  = owner;
	t.type          = ttVirtual;
	if (!title.isNull())
		t.internalTabWidget->setWindowTitle(title);
	else
		t.internalTabWidget->setWindowTitle(t.userTabOwner->windowTitle());
	HWND afterHwnd = before ? reinterpret_cast<HWND>(m_tabs.internal(before)->winId()) : 0;
	ForceIconicRepresentation((HWND)t.internalTabWidget->winId());
	m_tabs.append(t);
	RegisterTab(t.tabHandle, (HWND)owner->winId());
	SetTabOrder(t.tabHandle, afterHwnd);
	return t.id;
}

void TaskbarPreviews::setTabTitle(QWidget *tab, const QString &customTitle)
{
	QWidget *w = m_tabs.internal(tab);
	if (w)
		w->setWindowTitle(customTitle);
}

void TaskbarPreviews::setTabTitle(unsigned tabid, const QString &title)
{
	QWidget *w = m_tabs.internal(tabid);
	if (w)
		w->setWindowTitle(title);
}

void TaskbarPreviews::removeTab(QWidget *userWidgetTab)
{
	int i = 0;
	while(i < m_tabs.size()){
		_tab t = m_tabs.at(i);
		if(t.userTabWidget == userWidgetTab){
			UnregisterTab((HWND)t.internalTabWidget->winId());
			t.internalTabWidget->deleteLater();
			m_tabs.removeAt(i);
		}else
			i++;
	}
}

void TaskbarPreviews::removeTab(unsigned id)
{
	int i = 0;
	while (i < m_tabs.size()) {
		_tab t = m_tabs.at(i);
		if (t.id == id) {
			UnregisterTab((HWND)t.internalTabWidget->winId());
			t.internalTabWidget->deleteLater();
			m_tabs.removeAt(i);
		} else
			i++;
	}
}

void TaskbarPreviews::activateTab(QWidget *tab)
{
	QWidget *internal = m_tabs.internal(tab);
	if(internal)
		SetTabActive((HWND)internal->winId(), (HWND)m_tabs.owner((HWND)internal->winId())->winId());
}

void TaskbarPreviews::activateTab(unsigned id)
{
	QWidget *internal = m_tabs.internal(id);
	if(internal)
		SetTabActive((HWND)internal->winId(), (HWND)m_tabs.owner((HWND)internal->winId())->winId());
}

void TaskbarPreviews::changeOrder(QWidget *tab, QWidget *before)
{
	QWidget *inttab    = m_tabs.internal(tab);
	QWidget *intbefore = m_tabs.internal(before);
	if(!(inttab && intbefore))
		return;
	SetTabOrder((HWND)inttab->winId(), (HWND)intbefore->winId());
}

void TaskbarPreviews::changeOrder(QWidget *tab, unsigned beforeid)
{
	QWidget *inttab    = m_tabs.internal(tab);
	QWidget *intbefore = m_tabs.internal(beforeid);
	if(!(inttab && intbefore))
		return;
	SetTabOrder((HWND)inttab->winId(), (HWND)intbefore->winId());
}

void TaskbarPreviews::changeOrder(unsigned tabid, QWidget *before)
{
	QWidget *inttab    = m_tabs.internal(tabid);
	QWidget *intbefore = m_tabs.internal(before);
	if(!(inttab && intbefore))
		return;
	SetTabOrder((HWND)inttab->winId(), (HWND)intbefore->winId());
}

void TaskbarPreviews::changeOrder(unsigned tabid, unsigned beforeid)
{
	QWidget *inttab   = m_tabs.internal(tabid);
	QWidget *intbefore = m_tabs.internal(beforeid);
	if(!(inttab && intbefore))
		return;
	SetTabOrder((HWND)inttab->winId(), (HWND)intbefore->winId());
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
		InvalidateBitmaps((HWND)w->winId());
}

void TaskbarPreviews::refreshPreviews(unsigned tabid)
{
	QWidget *w = m_tabs.internal(tabid);
	if(w)
		InvalidateBitmaps((HWND)w->winId());
}

// Static functions

unsigned TaskbarPreviews::tabAdd(QWidget *tab, QWidget *owner, const QString &title, QWidget *before, PreviewProvider *pp)
{
	return TaskbarPreviews::instance()->addTab(tab, owner, title, before, pp);
}

unsigned TaskbarPreviews::tabAddVirtual(PreviewProvider *pp, QWidget *owner, const QString &title, QWidget *before)
{
	return TaskbarPreviews::instance()->addVirtualTab(pp, owner, title, before);
}

void TaskbarPreviews::tabActivate(QWidget *tab)
{
	TaskbarPreviews::instance()->activateTab(tab);
}

void TaskbarPreviews::tabActivate(unsigned tabid)
{
	TaskbarPreviews::instance()->activateTab(tabid);
}

void TaskbarPreviews::tabRemove(QWidget *tab)
{
	TaskbarPreviews::instance()->removeTab(tab);
}

void TaskbarPreviews::tabRemove(unsigned tabid)
{
	TaskbarPreviews::instance()->removeTab(tabid);
}

void TaskbarPreviews::tabOrderChange(QWidget *tab, QWidget *before)
{
	TaskbarPreviews::instance()->changeOrder(tab, before);
}

void TaskbarPreviews::tabOrderChange(QWidget *tab, unsigned beforeid)
{
	TaskbarPreviews::instance()->changeOrder(tab, beforeid);
}

void TaskbarPreviews::tabOrderChange(unsigned tabid, QWidget *before)
{
	TaskbarPreviews::instance()->changeOrder(tabid, before);
}

void TaskbarPreviews::tabOrderChange(unsigned tabid, unsigned beforeid)
{
	TaskbarPreviews::instance()->changeOrder(tabid, beforeid);
}

void TaskbarPreviews::tabsClear()
{
	TaskbarPreviews::instance()->clear();
}

void TaskbarPreviews::tabPreviewsRefresh(QWidget *tab)
{
	TaskbarPreviews::instance()->refreshPreviews(tab);
}

void TaskbarPreviews::tabPreviewsRefresh(unsigned tabid)
{
	TaskbarPreviews::instance()->refreshPreviews(tabid);
}

void TaskbarPreviews::tabSetTitle(QWidget *tab, const QString &customTitle)
{
	TaskbarPreviews::instance()->setTabTitle(tab, customTitle);
}

void TaskbarPreviews::tabSetTitle(unsigned tabid, const QString &title)
{
	TaskbarPreviews::instance()->setTabTitle(tabid, title);
}

void TaskbarPreviews::setWindowAttributes(QWidget *window, WindowAttributes wa)
{
	if (!window)
		return;
	SetWindowAttributes((HWND)window->winId(), wa);
}

// Functions, used by messages filter.

QPixmap TaskbarPreviews::IconicThumbnail(HWND hwnd, QSize size)
{
	QPixmap pixmap;
	TabType tt = m_tabs.type(hwnd);
	if (tt != ttVirtual) {
		QWidget *w = m_tabs.user(hwnd);
		PreviewProvider *pp = m_tabs.previews(hwnd);
		if (w) {
			if(pp)
				pixmap = pp->IconicPreview(w, m_tabs.owner(hwnd), size);
			else
				pixmap = QPixmap::grabWidget(w).scaled(size, Qt::KeepAspectRatio);
		}
	} else {
		unsigned wid = m_tabs.id(hwnd);
		PreviewProvider *pp = m_tabs.previews(hwnd);
		if(wid)
			pixmap = pp->IconicPreview(wid, m_tabs.owner(hwnd), size);
	}
	return pixmap;
}

QPixmap TaskbarPreviews::IconicLivePreviewBitmap(HWND hwnd)
{
	QPixmap pixmap;
	TabType tt = m_tabs.type(hwnd);
	if (tt != ttVirtual) {
		QWidget *w = m_tabs.user(hwnd);
		PreviewProvider *pp = m_tabs.previews(hwnd);
		if (w) {
			if(pp)
				pixmap = pp->LivePreview(w, m_tabs.owner(hwnd));
			else
				pixmap = QPixmap::grabWidget(m_tabs.owner(hwnd));
		}
	} else {
		unsigned wid = m_tabs.id(hwnd);
		PreviewProvider *pp = m_tabs.previews(hwnd);
		if (wid)
			pixmap = pp->LivePreview(wid, m_tabs.owner(hwnd));
	}
	return pixmap;
}

bool TaskbarPreviews::WasTabActivated(HWND hwnd)
{
	QWidget *internal = m_tabs.internal(hwnd);
	if (internal) {
		QWidget *owner = m_tabs.owner(hwnd);
		SetTabActive((HWND)internal->winId(), (HWND)owner->winId());
		if (owner->isMinimized())
			owner->showNormal();
		qApp->setActiveWindow(owner);
		emit tabActivated(m_tabs.user(hwnd));
		return true;
	} else
		return false;
}

bool TaskbarPreviews::WasTabRemoved(HWND hwnd)
{
	QWidget *internal = m_tabs.internal(hwnd);
	if (internal) {
		QWidget *owner = m_tabs.owner(hwnd);
		bool ignore = false;
		emit tabAboutToRemove(m_tabs.user(hwnd), &ignore);
		if (!ignore) {
			SetNoTabActive((HWND)owner->winId());
			removeTab(m_tabs.user(hwnd));
		}
		return true;
	} else
		return false;
}

///////////////////////////////////////////////////////////////////////////////
// Functions of TabsList --­ tabs info container.

QWidget *TaskbarPreviews::TabsList::internal(HWND internalHwnd)
{
	foreach(const TaskbarPreviews::_tab &t, *this){
		if(static_cast<void *>((HWND)t.internalTabWidget->winId()) == static_cast<void *>(internalHwnd))
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

QWidget *TaskbarPreviews::TabsList::internal(unsigned id)
{
	foreach(const TaskbarPreviews::_tab &t, *this){
		if(t.id == id)
			return t.internalTabWidget;
	}
	return 0;
}

QWidget *TaskbarPreviews::TabsList::user(HWND internalHwnd)
{
	foreach(const TaskbarPreviews::_tab &t, *this){
		if(static_cast<void *>((HWND)t.internalTabWidget->winId()) == static_cast<void *>(internalHwnd))
			return t.userTabWidget;
	}
	return 0;
}

unsigned TaskbarPreviews::TabsList::id(HWND internalHwnd)
{
	foreach(const TaskbarPreviews::_tab &t, *this){
		if(static_cast<void *>((HWND)t.internalTabWidget->winId()) == static_cast<void *>(internalHwnd))
			return t.id;
	}
	return 0;
}

TaskbarPreviews::TabType TaskbarPreviews::TabsList::type(HWND internalHwnd)
{
	foreach(const TaskbarPreviews::_tab &t, *this){
		if(static_cast<void *>((HWND)t.internalTabWidget->winId()) == static_cast<void *>(internalHwnd))
			return t.type;
	}
	return ttUnknown;
}

QWidget *TaskbarPreviews::TabsList::owner(HWND internalHandle)
{
	foreach(const TaskbarPreviews::_tab &t, *this){
		if(static_cast<void *>((HWND)t.internalTabWidget->winId()) == static_cast<void *>(internalHandle))
			return t.userTabOwner;
	}
	return 0;
}

void TaskbarPreviews::TabsList::insert(QWidget *internal, QWidget *userTab, QWidget *tabOwner, PreviewProvider *pp)
{
	TaskbarPreviews::_tab t;
	connect(userTab, SIGNAL(destroyed()), TaskbarPreviews::instance(), SLOT(widgetDestroyed()));
	t.internalTabWidget = internal;
	t.userTabWidget     = userTab;
	t.userTabOwner      = tabOwner;
	t.tabHandle         = reinterpret_cast<HWND>(internal->winId());
	t.pp                = pp;
	QList<_tab>::insert(0, t);
}

PreviewProvider *TaskbarPreviews::TabsList::previews(HWND internalHandle)
{
	foreach(const TaskbarPreviews::_tab &t, *this){
		if(static_cast<void *>((HWND)t.internalTabWidget->winId()) == static_cast<void *>(internalHandle))
			return t.pp;
	}
	return 0;
}



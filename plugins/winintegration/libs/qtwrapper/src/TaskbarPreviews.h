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

#ifndef TASKBARPREVIEWS_H
#define TASKBARPREVIEWS_H

#include <QWidget>
#include <QMap>
#include <QSize>
#include <QFlags>
#include <qt_windows.h>
#include "../../apilayer/src/ApiTaskbarPreviewsWAttributes.h"

Q_DECLARE_FLAGS(WindowAttributes, WindowAttribute)
Q_DECLARE_OPERATORS_FOR_FLAGS(WindowAttributes)

class PreviewProvider : public QObject {
	Q_OBJECT

public:
	virtual QPixmap IconicPreview(QWidget *tab, QWidget *owner, QSize)
	{
		Q_UNUSED(tab);
		Q_UNUSED(owner);
		return QPixmap();
	}

	virtual QPixmap LivePreview(QWidget *tab, QWidget *owner)
	{
		Q_UNUSED(tab);
		Q_UNUSED(owner);
		return QPixmap();
	}

	virtual QPixmap IconicPreview(unsigned tabid, QWidget *owner, QSize)
	{
		Q_UNUSED(tabid);
		Q_UNUSED(owner);
		return QPixmap();
	}

	virtual QPixmap LivePreview(unsigned tabid, QWidget *owner)
	{
		Q_UNUSED(tabid);
		Q_UNUSED(owner);
		return QPixmap();
	}
};

class TaskbarPreviews : public QObject
{
	Q_OBJECT

	enum TabType {
		ttNotSet = 0,
		ttWidget, // is a widget
		ttWindow, // is a window
		ttVirtual, // has no widget/window so PP must be specified
		ttUnknown = 100500
	};

	struct _tab
	{
		_tab();
		HWND tabHandle;
		QWidget *internalTabWidget;
		QWidget *userTabWidget;
		QWidget *userTabOwner;
		PreviewProvider *pp;
		TabType type;
		unsigned id;
	};

	class TabsList : public QList<_tab>
	{
	public:
		//TabsList();
		void insert(QWidget *internal, QWidget *userTab, QWidget *tabOwner, PreviewProvider *pp = 0);
		QWidget *internal(HWND internalHwnd);
		QWidget *internal(QWidget *user);
		QWidget *internal(unsigned id);
		TabType type(HWND internalHwnd);
		QWidget *user(HWND internalHwnd);
		unsigned id(HWND internalHwnd);
		QWidget *owner(HWND internalHwnd);
		PreviewProvider *previews(HWND internalHwnd);
	};

	TabsList m_tabs;

	TaskbarPreviews();

	friend class Win7EventFilter;
	QPixmap IconicThumbnail(HWND, QSize);
	QPixmap IconicLivePreviewBitmap(HWND);
	bool WasTabActivated(HWND);
	bool WasTabRemoved(HWND);

public:
	static TaskbarPreviews *instance();
	static unsigned tabAdd    (QWidget *tab, QWidget *owner, const QString &title = "", QWidget *before = 0, PreviewProvider *pp = 0);
	static unsigned tabAddVirtual(PreviewProvider *pp, QWidget *owner, const QString &title, QWidget *before = 0);
	static void setWindowAttributes(QWidget *window, WindowAttributes wa);
	static void tabActivate   (QWidget *tab);
	static void tabActivate   (unsigned tabid);
	static void tabOrderChange(QWidget *tab,   QWidget *before);
	static void tabOrderChange(QWidget *tab,   unsigned beforeid);
	static void tabOrderChange(unsigned tabid, QWidget *before);
	static void tabOrderChange(unsigned tabid, unsigned beforeid);
	static void tabPreviewsRefresh(QWidget *tab);
	static void tabPreviewsRefresh(unsigned tabid);
	static void tabRemove     (QWidget *tab);
	static void tabRemove     (unsigned tabid);
	static void tabsClear();
	static void tabSetTitle   (QWidget *tab,   const QString &customTitle);
	static void tabSetTitle   (unsigned tabid, const QString &title);

public slots:
	unsigned addTab (QWidget *tab, QWidget *owner, const QString &title = "", QWidget *before = 0, PreviewProvider *pp = 0);
	unsigned addVirtualTab(PreviewProvider *pp, QWidget *owner, const QString &title = "", QWidget *before = 0);
	void activateTab(QWidget *tab);
	void activateTab(unsigned tabid);
	void changeOrder(QWidget *tab,   QWidget *before);
	void changeOrder(QWidget *tab,   unsigned beforeid);
	void changeOrder(unsigned tabid, QWidget *before);
	void changeOrder(unsigned tabid, unsigned beforeid);
	void clear();
	void refreshPreviews(QWidget *tab);
	void refreshPreviews(unsigned tabid);
	void removeTab  (QWidget *tab);
	void removeTab  (unsigned tabid);
	void setTabTitle(QWidget *tab,   const QString &customTitle);
	void setTabTitle(unsigned tabid, const QString &title);
	void widgetDestroyed();

signals:
	void tabAboutToRemove(QWidget *tab, bool *ignore);
	void tabAboutToRemove(unsigned id,  bool *ignore);
	void tabActivated(QWidget *tab);
	void tabActivated(unsigned id);
};

#endif // TASKBARPREVIEWS_H


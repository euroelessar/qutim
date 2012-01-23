/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#ifndef TOOLFRAMEWINDOW_P_H
#define TOOLFRAMEWINDOW_P_H
#include "toolframewindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <qt_windows.h>
#include <QPointer>
#include <qutim/qtwin.h>
#include <QHash>
#include <QToolButton>
#include <QTimer>
#include "framebutton.h"

//class CaptionWidget : public QWidget
//{
//	Q_OBJECT
//public:
//	QWidget(QWidget *parent = 0) : QWidget(parent)
//	{

//	}

//};

using namespace qutim_sdk_0_3;

class QAction;
class ToolFrameWindow;
class ToolFrameWindowPrivate
{
	Q_DECLARE_PUBLIC(ToolFrameWindow)
public:
	ToolFrameWindowPrivate(ToolFrameWindow *q) : q_ptr(q) {}
	void loadThemeParams()
	{
		horizontalBorder = GetSystemMetrics(SM_CYFRAME);
		verticalBorder = GetSystemMetrics(SM_CXFRAME);
		captionHeight   = GetSystemMetrics(SM_CYCAPTION);
	}
	quintptr nativeNcHitTest(MSG *msg)
	{
		Q_Q(ToolFrameWindow);
		QPoint mouseHit(LOWORD(msg->lParam), HIWORD(msg->lParam));
		RECT windowRect = {0};
		GetWindowRect(q->winId(), &windowRect);

		int row = 1;
		int col = 1;
		bool onBorder = false;

		const int LeftOrTop = 0;
		const int RightOrBottom = 2;

		if (mouseHit.y() >= windowRect.top && mouseHit.y() < windowRect.top+horizontalBorder+captionHeight) {
			onBorder = (mouseHit.y() < windowRect.top + verticalBorder);
			row = LeftOrTop;
		} else if (mouseHit.y() < windowRect.bottom && mouseHit.y() >= windowRect.bottom - horizontalBorder) {
			row = RightOrBottom;
		}

		if (mouseHit.x() >= windowRect.left && mouseHit.x() < windowRect.left+ verticalBorder) {
			col = LeftOrTop;
		}
		else if (mouseHit.x() < windowRect.right && mouseHit.x() >= windowRect.right - verticalBorder) {
			col = RightOrBottom;
		}

		quintptr hitTests[3][3] =
		{
			{ HTTOPLEFT,    onBorder ? HTTOP : HTCAPTION,    HTTOPRIGHT },
			{ HTLEFT,       HTNOWHERE,     HTRIGHT },
			{ HTBOTTOMLEFT, HTBOTTOM, HTBOTTOMRIGHT },
		};

		return hitTests[row][col];
	}
	bool nativeNcCalcSize(MSG *msg, long *result)
	{
		//msg;
		*result = 0;
		return true;
	}
	void updateButtons()
	{
		foreach (QToolButton *btn, buttonHash) {
			btn->setIconSize(iconSize);
		}
		QTimer::singleShot(0, q_func(), SLOT(_q_do_layout()));
	}
	void _q_do_layout()
	{
		if (!(flags & ToolFrameWindow::DisableExtendFrame)) {
			int height = qMax(captionHeight, hLayout->sizeHint().height());
			QtWin::extendFrameIntoClientArea(q_func(), verticalBorder,
											 verticalBorder,
											 height,
											 horizontalBorder);
		}
	}
	QToolButton *createButton(QAction* action)
	{
		QToolButton *btn = new QToolButton(q_func());
		btn->setDefaultAction(action);
		btn->setAutoRaise(true);
		btn->setIconSize(iconSize);
		buttonHash.insert(action, btn);
		return btn;
	}

	ToolFrameWindow *q_ptr;
	QPointer<QWidget> centralWidget;
	QPointer<QMenu> menu;
	QPointer<FrameButton> frameButton;
	int horizontalBorder;
	int verticalBorder;
	int captionHeight;
	QVBoxLayout *vLayout;
	QHBoxLayout *hLayout;
	QSpacerItem *spacerItem;
	QSize iconSize;
	QHash<QAction*, QToolButton*> buttonHash;
	int flags;
};


#endif // TOOLFRAMEWINDOW_P_H


/****************************************************************************
**
** qutIM instant messenger
**
** Copyright (C) 2011 Aleksey Sidorov <sauron@citadelspb.com>
** Copyright (C) 2011 Ivan Vizir <define-true-false@yandex.com>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program. If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#include "toolframewindow_p.h"
#include <QLibrary>
#include <QStyle>
#include <QFrame>
#include <QAction>
#include <QShowEvent>
#include <QMenu>

typedef HRESULT (WINAPI * DwmDefWindowProc_t)(HWND, UINT, WPARAM, LPARAM, long*);
DwmDefWindowProc_t dwmDefWindowProc = 0;

ToolFrameWindow::ToolFrameWindow(int flags) :
	d_ptr(new ToolFrameWindowPrivate(this))
{
	Q_D(ToolFrameWindow);
	if (!dwmDefWindowProc) {
		QLibrary dwmapi("dwmapi");
		dwmDefWindowProc = reinterpret_cast<DwmDefWindowProc_t>(dwmapi.resolve("DwmDefWindowProc"));
	}
	d->flags = flags;
	d->loadThemeParams();

	int size = style()->pixelMetric(QStyle::PM_SmallIconSize);
	d->iconSize = QSize(size, size);

	d->vLayout = new QVBoxLayout(this);
	d->hLayout = new QHBoxLayout();
	d->vLayout->addLayout(d->hLayout);

	d->spacerItem = new QSpacerItem(0, d->captionHeight + d->verticalBorder, QSizePolicy::Expanding, QSizePolicy::Fixed);
	d->hLayout->addSpacerItem(d->spacerItem);

	addSpace(50); //FIXME find this size

	d->vLayout->setContentsMargins(d->verticalBorder, 0, d->verticalBorder, d->horizontalBorder);
	d->hLayout->setSpacing(0);
	d->vLayout->setSpacing(0);
	d->_q_do_layout();
}

ToolFrameWindow::~ToolFrameWindow()
{

}

void ToolFrameWindow::addAction(QAction *action)
{
	Q_D(ToolFrameWindow);
	QWidget::addAction(action);
	if (action->isSeparator())
		addSeparator();
	else
		addWidget(d->createButton(action), Qt::AlignVCenter);
}

void ToolFrameWindow::removeWidget(QWidget *widget)
{
	Q_D(ToolFrameWindow);
	d->hLayout->removeWidget(widget);
	d->_q_do_layout();
}

void ToolFrameWindow::removeAction(QAction *action)
{
	Q_D(ToolFrameWindow);
	QWidget::removeAction(action);
	d->buttonHash.take(action)->deleteLater();
	d->_q_do_layout();
}

void ToolFrameWindow::addWidget(QWidget *widget, Qt::Alignment aligment)
{
	Q_D(ToolFrameWindow);
	d->hLayout->insertWidget(d->hLayout->count() - 2, widget, 0, aligment);
	d->_q_do_layout();
	connect(widget, SIGNAL(destroyed()), this, SLOT(_q_do_layout()));
}

void ToolFrameWindow::setCentralWidget(QWidget *widget)
{
	Q_D(ToolFrameWindow);
	if (d->centralWidget) {
		layout()->removeWidget(d->centralWidget);
		d->centralWidget->removeEventFilter(this);
		foreach (QAction *action, actions())
			removeAction(action);
	}
	d->centralWidget = widget;
	d->centralWidget->winId();
	layout()->addWidget(widget);
	setGeometry(widget->geometry());
	d->centralWidget->setAutoFillBackground(false);
	if (d->flags & MergeWidgetActions)
		addActions(widget->actions());

	d->_q_do_layout();
}

void ToolFrameWindow::setIconSize(const QSize &size)
{
	Q_D(ToolFrameWindow);
	d->iconSize = size;
	d->updateButtons();
}

QSize ToolFrameWindow::iconSize() const
{
	return d_func()->iconSize;
}

bool ToolFrameWindow::winEvent(MSG *msg, long *result)
{
	//return QWidget::winEvent(msg, result); //Viv почему кнопки не жмакаются?
	Q_D(ToolFrameWindow);
	if (!QtWin::isCompositionEnabled())
		return QWidget::winEvent(msg, result);

	if (dwmDefWindowProc(msg->hwnd, msg->message, msg->wParam, msg->lParam, result))
		return true;

	switch (msg->message) {
	case WM_SHOWWINDOW: {
		RECT rc;
		GetWindowRect(winId(), &rc);
		SetWindowPos(winId(),
					 NULL,
					 rc.left, rc.top,
					 rc.right-rc.left, rc.bottom-rc.top,
					 SWP_FRAMECHANGED);
		return false;
	}
	case WM_NCCALCSIZE:
		*result = d->nativeNcCalcSize(msg, result);
		return true;
	case WM_NCHITTEST:
		*result = d->nativeNcHitTest(msg);
		return true;
	default:
		return false;
	}
}

QWidget *ToolFrameWindow::addSeparator()
{
	QFrame *frame = new QFrame(this);
	frame->setLineWidth(1);
	frame->setMinimumSize(d_func()->iconSize.width() / 2,
						  d_func()->iconSize.height() * 0.75);
	frame->setFrameShape(QFrame::VLine);
	frame->setFrameShadow(QFrame::Plain);

	QPalette palete = frame->palette();
	palete.setColor(QPalette::WindowText,
					palete.color(QPalette::Shadow));
	frame->setPalette(palete);

	addWidget(frame, Qt::AlignCenter);
	return frame;
}

void ToolFrameWindow::insertAction(QAction *before, QAction *action)
{
	Q_D(ToolFrameWindow);
	QToolButton *btn = d->buttonHash.value(before);
	int index = d->hLayout->indexOf(btn);
	d->hLayout->insertWidget(index, d->createButton(action), 0, Qt::AlignVCenter);
}

QWidget *ToolFrameWindow::addSpace(int size)
{
	QWidget *spacer = new QWidget(this);
	spacer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	spacer->setMinimumSize(size, 0);
	spacer->setMaximumWidth(size);
	spacer->setAttribute(Qt::WA_TransparentForMouseEvents);
	addWidget(spacer);
	return spacer;
}

bool ToolFrameWindow::eventFilter(QObject *o, QEvent *e)
{
	return QWidget::eventFilter(o, e);
}

void ToolFrameWindow::setMenu(QMenu *menu)
{
	Q_D(ToolFrameWindow);
	if (d->menu)
		d->menu->deleteLater();
	if (!menu) {
		d->frameButton->deleteLater();
		return;
	}

	QIcon icon = menu->icon();
	if (icon.isNull() && d->centralWidget)
		icon = d->centralWidget->windowIcon();
	else
		icon = windowIcon();

	if(!d->frameButton) {
		d->frameButton = new FrameButton(this);
		d->hLayout->insertWidget(0, d->frameButton, 0, Qt::AlignTop);
	}

	d->frameButton->setMenu(menu);
	d->frameButton->setIcon(icon);
	if (0)
		d->frameButton->setText(menu->title());
	d->menu = menu;
}

QMenu *ToolFrameWindow::menu() const
{
	return d_func()->menu;
}

//#include "moc_toolframewindow.cpp" //for qmake users
#include "toolframewindow.moc" //for cmake users


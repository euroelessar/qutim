/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <sauron@citadelspb.com>
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

#ifndef TOOLFRAMEWINDOW_H
#define TOOLFRAMEWINDOW_H

#include <QWidget>

class QMenu;
class ToolFrameWindowPrivate;
class ToolFrameWindow : public QWidget
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(ToolFrameWindow);
public:
	enum Flags
	{
		DisableExtendFrame =	0x1, /*! For manual control */
		MergeWidgetActions =	0x2, /*! Take QWidget::actions from centralWidget */
	};
	explicit ToolFrameWindow(int flags = MergeWidgetActions);
	virtual ~ToolFrameWindow();
	void addAction(QAction *action);
	QWidget *addSeparator();
	QWidget *addSpace(int size);
	void addWidget(QWidget *widget, Qt::Alignment = Qt::AlignTop);
	void insertAction(QAction *before, QAction *action);
	//QWidget *insertSeparator(QAction *before);
	void removeAction(QAction *action);
	void removeWidget(QWidget *widget);
	void setCentralWidget(QWidget *widget);
	/*! Draw in caption nice vista style button with popup menu
	  */
	void setMenu(QMenu *menu);
	QMenu *menu() const;
	void setIconSize(const QSize &size);
	QSize iconSize() const;
protected:
	bool winEvent(MSG *message, long *result);
	bool eventFilter(QObject *o, QEvent *e);
private:
	QScopedPointer<ToolFrameWindowPrivate> d_ptr;
	Q_PRIVATE_SLOT(d_func(), void _q_do_layout())
};


#endif // TOOLFRAMEWINDOW_H


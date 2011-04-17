/****************************************************************************
 *  toolframewindow.h
 *
 *  Copyright (c) 2011 by Sidorov Aleksey <sauron@citadelspb.com>
 *  Copyright (c) 2011 by Vizir Ivan <define-true-false@yandex.com >
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

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

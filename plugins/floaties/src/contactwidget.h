/*
    Floaties

	Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#ifndef CONTACTWIDGET_H
#define CONTACTWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QContextMenuEvent>
#include <qutim/contact.h>
#include <QAbstractItemDelegate>

class FloatiesPlugin;

class ContactWidget : public QWidget
{
    Q_OBJECT
public:
	ContactWidget(const QPersistentModelIndex &index, QAbstractItemView *view,
				  qutim_sdk_0_3::Contact *contact);
	~ContactWidget();
    virtual void contextMenuEvent(QContextMenuEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
	virtual void paintEvent(QPaintEvent *event);
signals:
	void wantDie(QObject *obj);
protected:
	virtual bool event(QEvent *event);
	void ensureSize();
	QStyleOptionViewItemV4 viewOptionV4();
private:
	QPersistentModelIndex m_index;
	QAbstractItemView *m_view;
	qutim_sdk_0_3::Contact *m_contact;
    FloatiesPlugin *m_plugin;
    bool m_mouse_pressed;
    QPoint m_position;
};

#endif // CONTACTWIDGET_H

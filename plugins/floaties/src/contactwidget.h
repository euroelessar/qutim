/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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


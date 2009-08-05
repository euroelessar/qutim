/*****************************************************************************
	QtCustomWidget

	Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#ifndef QTCUSTOMWIDGET_P_H
#define QTCUSTOMWIDGET_P_H

#include "qtcustomwidget.h"

QT_BEGIN_NAMESPACE

class QtCustomButtonPrivate;
class QPixmap;

struct QtCustomButtonGeometry
{
	enum Type
	{
		LeftToCenter,
		Center,
		RightToCenter
	};
	QtCustomButtonGeometry()
		: left_m(0), left_t(LeftToCenter), top_m(0), top_t(LeftToCenter),
		right_m(0), right_t(LeftToCenter), bottom_m(0), bottom_t(LeftToCenter) {}
	int left_m;
	Type left_t;
	int top_m;
	Type top_t;
	int right_m;
	Type right_t;
	int bottom_m;
	Type bottom_t;
};

class QtCustomButton : public QWidget
{
	Q_OBJECT
public:
	enum State
	{
		Hover,
		Pressed,
		Normal
	};
	enum Action
	{
		Minimize,
		Close
	};
	QtCustomButton(const QtCustomButtonGeometry &geometry, Action action, QWidget *parent);
	const QtCustomButtonGeometry &getGeometry();
	void setPixmaps(const QPixmap &normal, const QPixmap &hover);
	virtual ~QtCustomButton();
protected:
	virtual void paintEvent(QPaintEvent *event);
	virtual void leaveEvent(QEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);
	void setState(State state);
private:
	QtCustomButtonPrivate *p;
};

QT_END_NAMESPACE

#endif // QTCUSTOMWIDGET_P_H

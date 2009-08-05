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

#ifndef QTCUSTOMWIDGET_H
#define QTCUSTOMWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE

class QEvent;
struct QtCustomWidgetPrivate;

class QtCustomWidget : public QObject
{
	Q_OBJECT
public:
	QtCustomWidget(QWidget *widget);
	virtual ~QtCustomWidget();
	void start(const QString &config_path);
	void stop();
protected:
	virtual bool eventFilter(QObject *obj, QEvent *event);
private:
	QtCustomWidgetPrivate *p;
};

QT_END_NAMESPACE

#endif // QTCUSTOMWIDGET_H

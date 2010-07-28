/****************************************************************************
 *  actionbox_p.h
 *
 *  Copyright (c) 2010 by Aleksey Sidorov <sauron@citadelspb.com>
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

#ifndef ACTIONBOX_P_H
#define ACTIONBOX_P_H
#include <QObject>
#include <QMap>

class ActionBox;
class QToolButton;
class QAction;
class QHBoxLayout;
class ActionBoxPrivate : public QObject
{
	Q_OBJECT
	Q_DECLARE_PUBLIC(ActionBox)
public:
	ActionBoxPrivate() : QObject(),q_ptr(0),layout(0) {}
	ActionBox *q_ptr;
	QMap<QAction*,QToolButton*> buttons;
	QHBoxLayout *layout;
public slots:
	void onButtonDestroyed(QObject *obj);
	void onChanged();
};

#endif // ACTIONBOX_P_H

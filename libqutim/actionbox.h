/****************************************************************************
 *  actionbox.h
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

#ifndef ACTIONBOX_H
#define ACTIONBOX_H

#include <QWidget>
#include "libqutim_global.h"

namespace qutim_sdk_0_3
{

class ActionBoxPrivate;
class LIBQUTIM_EXPORT ActionBox : public QWidget
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(ActionBox)
public:
	explicit ActionBox(QWidget *parent);
	void addAction(QAction *action);
	void addActions(QList<QAction*> actions);
	void removeAction(QAction *action);
	void removeActions(QList<QAction*> actions);
	void clear();
	~ActionBox();
private:
	QScopedPointer<ActionBoxPrivate> d_ptr;
};

class LIBQUTIM_EXPORT ActionBoxModule : public QWidget
{
	Q_OBJECT
	Q_CLASSINFO("Service","ActionBoxModule")
public:
	virtual void addAction(QAction *) = 0;
	virtual void removeAction(QAction *) = 0;
};

}

#endif // ACTIONBOX_H

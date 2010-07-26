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

class ActionBoxPrivate;
class ActionBox : public QWidget
{
	//simple soft key emulation
    Q_OBJECT
	Q_DECLARE_PRIVATE(ActionBox)
public:
	explicit ActionBox(QWidget *parent);
	void addAction(QAction *action);
	void addActions(QList<QAction*> actions);
	~ActionBox();
private:
	QScopedPointer<ActionBoxPrivate> d_ptr;
};

#endif // ACTIONBOX_H

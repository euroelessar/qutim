/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Sidorov Aleksey <sauron@citadelspb.com>
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
public:
	virtual void addAction(QAction *) = 0;
	virtual void removeAction(QAction *) = 0;
};

class LIBQUTIM_EXPORT ActionBoxGenerator : public QObject
{
	Q_OBJECT
	Q_CLASSINFO("Service","ActionBoxModule")
public:
	virtual ActionBoxModule *generate() = 0;
	virtual ~ActionBoxGenerator() {};
};

}

#endif // ACTIONBOX_H


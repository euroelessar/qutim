/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef ACTIONTOOLBAR_P_H
#define ACTIONTOOLBAR_P_H
#include <QObject>
#include <QPointer>
#include <QPoint>
#include <QVariant>
#include "localizedstring.h"
#include "menucontroller_p.h"
#include <QSize>

class QActionGroup;
class QMenu;
class QAction;

namespace qutim_sdk_0_3
{
class ActionToolBar;
class ActionGenerator;

typedef QList<ActionGenerator*> SizeList;

class ActionToolBarPrivate
{
	Q_DECLARE_PUBLIC(ActionToolBar)
public:
	ActionToolBarPrivate();
	virtual ~ActionToolBarPrivate();
	QMenu *initContextMenu();
	QList<ActionValue::Ptr> actions;
	QList<ObjectGenerator::Ptr> holders;
	QVariant data;
	QPoint dragPos;
	bool moveHookEnabled;
	ActionToolBar *q_ptr;
	QActionGroup *fillMenu(QMenu *menu, SizeList *map, int current = 0);
	QSize size;
	int style;
	void _q_size_action_triggered(QAction*);
	void _q_style_action_triggered(QAction*);
};
}

#endif // ACTIONTOOLBAR_P_H


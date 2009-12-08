/****************************************************************************
 *  menucontroller.cpp
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#include "menucontroller_p.h"
#include <QMap>

namespace qutim_sdk_0_3
{
	typedef QMap<const QMetaObject *, const ActionGenerator *> MenuActionMap;

	Q_GLOBAL_STATIC(MenuActionMap, globalActions)

	MenuController::MenuController(QObject *parent) : QObject(parent), d_ptr(new MenuControllerPrivate)
    {
    }

	MenuController::MenuController(MenuControllerPrivate &mup, QObject *parent) : QObject(parent), d_ptr(&mup)
	{
	}

	MenuController::~MenuController()
    {
    }

	inline bool actionLessThan(const ActionGenerator *a, const ActionGenerator *b)
	{
		if (a->type() == b->type()) {
			if (a->priority() == b->priority())
				return a->text() < b->text();
			else
				return a->priority() > b->priority();
		} else
			return a->type() < b->type();
	}

	QMenu *MenuController::menu(bool deleteOnClose) const
	{
		QMenu *menu = new QMenu();
		menu->setAttribute(Qt::WA_DeleteOnClose, deleteOnClose);
		QList<const ActionGenerator *> actions = d_func()->actions;
		const QMetaObject *meta = metaObject();
		while (meta) {
			actions.append(globalActions()->value(meta));
			meta = meta->superClass();
		}
		if (actions.isEmpty())
			return menu;
		qSort(actions.begin(), actions.end(), actionLessThan);
		int lastType = actions[0]->type();
		for (int i = 0; i < actions.size(); i++) {
			if (lastType != actions[i]->type()) {
				lastType = actions[i]->type();
				menu->addSeparator();
			}
			QAction *action = actions[i]->generate<QAction>();
			action->setParent(action);
			menu->addAction(action);
		}
		return menu;
	}

	void MenuController::addAction(const ActionGenerator *gen)
	{
		Q_ASSERT(gen);
		d_func()->actions.append(gen);
	}

	void MenuController::addAction(const ActionGenerator *gen, const QMetaObject *meta)
	{
		Q_ASSERT(gen && meta);
		globalActions()->insertMulti(meta, gen);
	}
}

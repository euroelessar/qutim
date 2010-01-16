/****************************************************************************
 *  actiontoolbar.cpp
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#include "actiontoolbar.h"
#include <QAction>

namespace qutim_sdk_0_3
{
	struct ActionToolBarPrivate
	{
		QList<ActionGenerator *> generators;
		QList<QPointer<QAction> > actions;
		QVariant data;
	};

	ActionToolBar::ActionToolBar(const QString &title, QWidget *parent)
			: QToolBar(title, parent), p(new ActionToolBarPrivate)
	{
	}

	ActionToolBar::ActionToolBar(QWidget *parent)
			: QToolBar(parent), p(new ActionToolBarPrivate)
	{
	}

	ActionToolBar::~ActionToolBar()
	{
	}

	QAction *ActionToolBar::addAction(ActionGenerator *generator)
	{
		Q_ASSERT(generator);
		int index = p->generators.indexOf(generator);
		if (index != -1)
			return p->actions.at(index);
		QAction *action = generator->generate<QAction>();
		action->setData(p->data);
		p->generators << generator;
		p->actions << action;
		QWidget::addAction(action);
		return action;
	}

	void ActionToolBar::setData(const QVariant &var)
	{
		if (p->data == var)
			return;
		for (int i = 0; i < p->actions.size(); i++) {
			if (p->actions.at(i).isNull()) {
				p->actions.removeAt(i);
				p->generators.removeAt(i);
				i--;
			} else
				p->actions.at(i)->setData(var);
		}
	}

	QVariant ActionToolBar::data() const
	{
		return p->data;
	}
}

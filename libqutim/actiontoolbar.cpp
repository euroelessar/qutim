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
#include <QToolButton>
#include <QMouseEvent>

namespace qutim_sdk_0_3
{
	struct ActionToolBarPrivate
	{
		QList<ActionGenerator *> generators;
		QList<QPointer<QAction> > actions;
		QVariant data;
		QPoint dragPos;
		bool moveHookEnabled;
	};

	ActionToolBar::ActionToolBar(const QString &title, QWidget *parent)
			: QToolBar(title, parent), p(new ActionToolBarPrivate)
	{
		p->moveHookEnabled = false;
	}

	ActionToolBar::ActionToolBar(QWidget *parent)
			: QToolBar(parent), p(new ActionToolBarPrivate)
	{
		p->moveHookEnabled = false;
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
		bool hasMenu = !!action->menu();
		QWidget::addAction(action);
		if (hasMenu) {
			QList<QToolButton *> buttons = findChildren<QToolButton*>();
			for (int i = buttons.size() - 1; i >= 0; i--) {
				if (buttons.at(i)->defaultAction() == action) {
					buttons.at(i)->setPopupMode(QToolButton::InstantPopup);
					break;
				}
			}
		}
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

	void ActionToolBar::mousePressEvent(QMouseEvent *event)
	{
		if(p->moveHookEnabled && event->button() == Qt::LeftButton)
			p->dragPos = event->globalPos() - QWidget::window()->frameGeometry().topLeft();
	}

	void ActionToolBar::mouseMoveEvent(QMouseEvent *event)
	{
		if(p->moveHookEnabled && event->buttons() & Qt::LeftButton)
			QWidget::window()->move(event->globalPos() - p->dragPos);
	}

	void ActionToolBar::setMoveHookEnabled(bool enabled)
	{
		p->moveHookEnabled = enabled;
	}
}

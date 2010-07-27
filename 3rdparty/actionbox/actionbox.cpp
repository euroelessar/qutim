/****************************************************************************
 *  actionbox.cpp
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

#include "actionbox.h"
#include <QAction>
#include <QPushButton>
#include <QHBoxLayout>
#include "actionbox_p.h"
#include <QDebug>

ActionBox::ActionBox(QWidget *parent) :
	QWidget(parent),d_ptr(new ActionBoxPrivate)
{
	Q_D(ActionBox);
	d->q_ptr = this;
	Q_ASSERT(parent);
#ifndef QUTIM_SOFTKEYS_SUPPORT
	d->layout = new QHBoxLayout(this);
	setLayout(d->layout);
	d->layout->setMargin(0);
#endif
}

void ActionBox::addAction(QAction *action)
{
	qDebug() << "add action" << action->text();
	Q_D(ActionBox);
#ifdef QUTIM_SOFTKEYS_SUPPORT
	static_cast<QWidget*>(parent())->addAction(action);
#else
	QPushButton *button = new QPushButton(this);
	button->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
	d->buttons.insert(action,button);
	d->updateButton(button,action);
	connect(action,SIGNAL(changed()),d,SLOT(onActionChanged()));
	connect(button,SIGNAL(clicked()),action,SIGNAL(triggered()));
	if (action->softKeyRole() == QAction::PositiveSoftKey)
		d->layout->insertWidget(0,button);
	else /*if (action->softKeyRole() == QAction::NegativeSoftKey)*/
		d->layout->addWidget(button);
//	TODO
//	else {
//		int index = d->layout->count() - 2;
//		if (index >= 0)
//			d->la
//	}
#endif
	QWidget::addAction(action);
}

void ActionBox::addActions(QList<QAction*> actions)
{
	foreach (QAction *action,actions)
		addAction(action);
}

ActionBox::~ActionBox()
{

}

void ActionBox::removeAction(QAction *action)
{
	#ifndef QUTIM_SOFTKEYS_SUPPORT
	Q_D(ActionBox);
	d->buttons.take(action)->deleteLater();
	#endif
	QWidget::removeAction(action);
}

void ActionBox::removeActions(QList<QAction *> actions)
{
	foreach (QAction *action,actions)
		removeAction(action);
}

void ActionBoxPrivate::updateButton(QPushButton *button,const QAction *action)
{
	button->setText(action->text());
	button->setIcon(action->icon());
	button->setToolTip(action->toolTip());
	button->setShortcut(action->shortcut());
	button->setMenu(action->menu());
}

void ActionBoxPrivate::onActionChanged()
{
	QAction *action = qobject_cast<QAction*>(sender());
	Q_ASSERT(action);
	QPushButton *button = buttons.value(action);
	Q_ASSERT(button);
	updateButton(button,action);
}

void ActionBoxPrivate::onActionDestroyed(QObject *obj)
{
	QAction *action = reinterpret_cast<QAction*>(obj);
	buttons.take(action)->deleteLater();
}



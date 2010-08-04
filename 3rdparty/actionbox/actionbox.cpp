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
#include <QToolButton>
#include <QHBoxLayout>
#include "actionbox_p.h"
#include <libqutim/libqutim_global.h>
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
	if (actions().contains(action))
		return;
	QToolButton *button = new QToolButton(this);
	button->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
	button->setDefaultAction(action);
	button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	d->buttons.insert(action,button);

	connect(action,SIGNAL(changed()),d,SLOT(onChanged()));
	connect(button,SIGNAL(destroyed(QObject*)),d,SLOT(onButtonDestroyed(QObject*)));
	connect(action,SIGNAL(destroyed()),button,SLOT(deleteLater()));

	if (action->softKeyRole() == QAction::NegativeSoftKey)
		d->layout->insertWidget(0,button);
	else
		d->layout->addWidget(button);
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

void ActionBoxPrivate::onButtonDestroyed(QObject *obj)
{
	QToolButton *button = reinterpret_cast<QToolButton*>(obj);
	QAction *action = buttons.key(button);
	buttons.remove(action);
}

void ActionBoxPrivate::onChanged()
{
	//small hack
	QAction *action = qobject_cast<QAction*>(sender());
	Q_ASSERT(action);
	QToolButton *button = buttons.value(action);
	Q_ASSERT(button);
	button->setVisible(action->isVisible());
}


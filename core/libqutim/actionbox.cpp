/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#include "actionbox.h"
#include <QAction>
#include <QToolButton>
#include <QHBoxLayout>
#include "actionbox_p.h"
#include "objectgenerator.h"
#include "debug.h"
#include "servicemanager.h"

namespace qutim_sdk_0_3
{

ActionBox::ActionBox(QWidget *parent) :
		QWidget(parent),d_ptr(new ActionBoxPrivate)
{
	Q_UNUSED(QT_TRANSLATE_NOOP("Service","ActionBoxModule"));
	Q_D(ActionBox);
	d->q_ptr = this;
	Q_ASSERT(parent);
	
	ServicePointer<ActionBoxGenerator> generator;
	if (generator)
		d->module = generator->generate();
	
	if(d->module) {
		d->module->setParent(this);
		setLayout(new QHBoxLayout(this));
		layout()->setMargin(0);
		layout()->addWidget(d->module);
	}
}

void ActionBox::addAction(QAction *action)
{
	Q_D(ActionBox);
	if(d->module) {
		d->module->addAction(action);
	}
	QWidget::addAction(action);
}

void ActionBox::addActions(QList<QAction*> actions)
{
	foreach (QAction *action,actions)
		addAction(action);
}

ActionBox::~ActionBox()
{
	d_func()->module->deleteLater();
}

void ActionBox::removeAction(QAction *action)
{
	Q_D(ActionBox);
	if(d->module)
		d->module->removeAction(action);
	QWidget::removeAction(action);
}

void ActionBox::removeActions(QList<QAction *> actions)
{
	foreach (QAction *action,actions)
		removeAction(action);
}

void ActionBox::clear()
{
	removeActions(actions());
}

}


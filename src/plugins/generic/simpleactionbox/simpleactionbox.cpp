/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#include "simpleactionbox.h"
#include <QHBoxLayout>
#include <QToolButton>
#include <QAction>
#include <qutim/debug.h>
#include <QHBoxLayout>

namespace Core
{

SimpleActionBoxModule::SimpleActionBoxModule()
{
	m_layout = new QHBoxLayout(this);
	m_layout->setMargin(0);
}

void SimpleActionBoxModule::addAction(QAction *action)
{
	qDebug() << "SimpleActionBox Added action" << action;
	if (actions().contains(action))
		return;

	QToolButton *button = new QToolButton(this);
	button->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
	button->setDefaultAction(action);
	button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	button->setVisible(action->isVisible());
	m_buttons.insert(action,button);

	connect(action,SIGNAL(changed()),SLOT(onChanged()));
	connect(button,SIGNAL(destroyed(QObject*)),SLOT(onButtonDestroyed(QObject*)));
	connect(action,SIGNAL(destroyed()),button,SLOT(deleteLater()));
	m_layout->addWidget(button);
	QWidget::addAction(action);
}

void SimpleActionBoxModule::removeAction(QAction *action)
{
	m_buttons.take(action)->deleteLater();
	QWidget::removeAction(action);
	disconnect(action, 0, this, 0);
}

void SimpleActionBoxModule::onButtonDestroyed(QObject *obj)
{
		QToolButton *button = reinterpret_cast<QToolButton*>(obj);
		QAction *action = m_buttons.key(button);
		m_buttons.remove(action);
}

void SimpleActionBoxModule::onChanged()
{
		QAction *action = qobject_cast<QAction*>(sender());
		Q_ASSERT(action);
		QToolButton *button = m_buttons.value(action);
		Q_ASSERT(button);
		button->setVisible(action->isVisible());
}

}


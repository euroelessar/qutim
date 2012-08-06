/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "autoreplybuttonaction.h"
#include "rostermanager.h"
#include <qutim/icon.h>
#include <qutim/history.h>
#include <qutim/notification.h>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QToolButton>

namespace Control
{

using namespace qutim_sdk_0_3;

AutoReplyButtonActionGenerator::AutoReplyButtonActionGenerator(QObject *object, const char *slot)
    : ActionGenerator(QIcon(), LocalizedString(), object, slot)
{
	setType(ActionTypeChatButton);
}

QObject *AutoReplyButtonActionGenerator::generateHelper() const
{
	return prepareAction(new AutoReplyButtonAction(NULL));
}

void AutoReplyButtonActionGenerator::createImpl(QAction *action, QObject *obj) const
{
	qobject_cast<AutoReplyButtonAction*>(action)->setController(obj);
}

void AutoReplyButtonActionGenerator::showImpl(QAction *action, QObject *obj)
{
	Q_UNUSED(action);
	Q_UNUSED(obj);
}

void AutoReplyButtonActionGenerator::hideImpl(QAction *action, QObject *obj)
{
	Q_UNUSED(action);
	Q_UNUSED(obj);
}

AutoReplyButtonAction::AutoReplyButtonAction(QObject *parent) :
    QWidgetAction(parent)
{
}

void AutoReplyButtonAction::setController(QObject *controller)
{
	m_controller = qobject_cast<Contact*>(controller);
}

QWidget *AutoReplyButtonAction::createWidget(QWidget *parent)
{
	return new AutoReplyButton(this, parent);
}

void AutoReplyButtonAction::deleteWidget(QWidget *widget)
{
	QWidgetAction::deleteWidget(widget);
}

AutoReplyButton::AutoReplyButton(QAction *action, QWidget *parent)
    : QWidget(parent), m_action(action)
{
	QHBoxLayout *layout = new QHBoxLayout(this);
	m_spinBox = new QSpinBox(this);
	m_spinBox->setMinimum(1);
	m_button = new QToolButton(this);
	m_button->setText(tr("Send"));
//	m_button->setIcon(Icon(""));
	layout->addWidget(m_spinBox);
	layout->addWidget(m_button);
	connect(m_button, SIGNAL(clicked()), SLOT(onButtonClicked()));
}

AutoReplyButton::~AutoReplyButton()
{
}

void AutoReplyButton::onButtonClicked()
{
	m_action->setProperty("__control_count", m_spinBox->value());
	m_action->trigger();
}

}

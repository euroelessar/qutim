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

AutoReplyButtonActionGenerator::AutoReplyButtonActionGenerator()
    : ActionGenerator(QIcon(), LocalizedString(), 0, 0)
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
	return new AutoReplyButton(parent, m_controller.data());
}

void AutoReplyButtonAction::deleteWidget(QWidget *widget)
{
	QWidgetAction::deleteWidget(widget);
}

AutoReplyButton::AutoReplyButton(QWidget *parent, Contact *contact)
    : QWidget(parent), m_contact(contact)
{
	qDebug() << Q_FUNC_INFO << parent << m_contact;
	QHBoxLayout *layout = new QHBoxLayout(this);
	m_spinBox = new QSpinBox(this);
	m_spinBox->setMinimum(1);
	m_button = new QToolButton(this);
	m_button->setIcon(Icon(""));
	layout->addWidget(m_spinBox);
	layout->addWidget(m_button);
	connect(m_button, SIGNAL(clicked()), SLOT(onButtonClicked()));
	MessageList messages = History::instance()->read(m_contact, 10);
	foreach (const Message &message, messages) {
		if (message.isIncoming())
			m_messages << message;
	}
}

AutoReplyButton::~AutoReplyButton()
{
	qDebug() << Q_FUNC_INFO << m_contact;
}

void AutoReplyButton::onButtonClicked()
{
	const MessageList messages = RosterManager::instance()->messages(m_contact);
	const int first = qMax(0, messages.size() - m_spinBox->value());
	QString text;
	for (int i = first; i < messages.size(); ++i) {
		const Message &message = messages.at(i);
		text += message.text();
		text += QLatin1Char('\n');
	}
	text.chop(1);
	NotificationRequest request(Notification::System);
	request.setTitle(tr("Control plugin"));
	request.setText(tr("qutIM sends request to server:\n%1").arg(text));
	request.send();
	RosterManager::instance()->networkManager()->sendRequest(m_contact, text);
}

}

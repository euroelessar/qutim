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

#include "vcontact.h"
#include "vaccount.h"
#include "vroster.h"
#include "vinforequest.h"

#include <qutim/tooltip.h>
#include <qutim/inforequest.h>
#include <qutim/notification.h>
#include <qutim/message.h>

#include <vk/contact.h>

#include <QTimer>

using namespace qutim_sdk_0_3;

VContact::VContact(vk::Buddy *contact, VAccount* account): Contact(account),
	m_buddy(contact)
{
}


QString VContact::id() const
{
	return QString::number(m_buddy->id());
}

bool VContact::isInList() const
{
	return m_buddy->isFriend();
}

bool VContact::sendMessage(const Message& message)
{
	if (!m_buddy->client()->isOnline())
		return false;
	m_buddy->sendMessage(message.text());
	return true;
}

void VContact::setTags(const QStringList& tags)
{
	Q_UNUSED(tags);
}

void VContact::setInList(bool inList)
{
	Q_UNUSED(inList);
}

Status VContact::status() const
{
	Status status = Status::instance(m_buddy->isOnline() ? Status::Online : Status::Offline, "vkontakte");
	status.setText(m_buddy->activity());
	return status;
}

QString VContact::activity() const
{
	return m_buddy->activity();
}

void VContact::setTyping(bool set)
{
	if (set) {
		if (m_typingTimer.isNull()) {
			m_typingTimer = new QTimer(this);
			m_typingTimer->setInterval(5000);
			connect(m_typingTimer, SIGNAL(timeout()), SLOT(setTyping()));
			connect(m_typingTimer, SIGNAL(timeout()), m_typingTimer, SLOT(deleteLater()));
		}
		m_typingTimer->start();
		setChatState(ChatStateComposing);
	} else
		setChatState(ChatStateActive);
}

VContact::~VContact()
{

}

QStringList VContact::tags() const
{
	return m_buddy->tags();
}

QString VContact::name() const
{
	return m_buddy->name();
}

void VContact::setName(const QString& name)
{
	Q_UNUSED(name);
}

QString VContact::avatar() const
{
	return m_buddy->photoSource();
}

bool VContact::event(QEvent *ev)
{
	if (ev->type() == ToolTipEvent::eventType()) {
		ToolTipEvent *event = static_cast<ToolTipEvent*>(ev);
		if (!activity().isEmpty())
			event->addField(QT_TRANSLATE_NOOP("ContactInfo", "Activity"),
							activity());
	}
	return Contact::event(ev);
}

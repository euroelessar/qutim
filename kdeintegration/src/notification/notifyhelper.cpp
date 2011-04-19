/*
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "notifyhelper.h"
#include <qutim/messagesession.h>
#include <QDebug>

NotifyHelper::NotifyHelper(ChatUnit *unit, const QVariant &data) : m_unit(unit)
{
	Q_UNUSED(data);
}

void NotifyHelper::registerNotification(KNotification* notification)
{
	m_notification = notification;
}

void NotifyHelper::startChatSlot()
{
	if (m_unit)
		ChatLayer::get(m_unit, true)->setActive(true);
}

void NotifyHelper::closeSlot()
{
}


NotifyHelper::~NotifyHelper()
{
	qDebug() << "Notification deleted";
}

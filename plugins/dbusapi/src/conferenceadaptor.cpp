/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "conferenceadaptor.h"
#include "chatunitadaptor.h"

ConferenceAdaptor::ConferenceAdaptor(const QDBusConnection &dbus, Conference *conf) :
		QDBusAbstractAdaptor(conf), m_dbus(dbus)
{
	connect(conf, SIGNAL(topicChanged(QString,QString)),
	        this, SIGNAL(topicChanged(QString)));
	connect(conf, SIGNAL(meChanged(qutim_sdk_0_3::Buddy*)),
			this, SLOT(onMeChanged(qutim_sdk_0_3::Buddy*)));
}

QDBusObjectPath ConferenceAdaptor::me() const
{
	Buddy *buddy = static_cast<Conference*>(parent())->me();
	QDBusObjectPath path = ChatUnitAdaptor::ensurePath(m_dbus, buddy);
	return path;
}

void ConferenceAdaptor::onMeChanged(qutim_sdk_0_3::Buddy *buddy)
{
	QDBusObjectPath path = ChatUnitAdaptor::ensurePath(m_dbus, buddy);
	emit meChanged(path);
}


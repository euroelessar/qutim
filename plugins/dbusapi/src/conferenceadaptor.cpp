/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
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

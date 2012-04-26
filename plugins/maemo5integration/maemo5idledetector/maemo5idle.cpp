/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Evgeniy Degtyarev <degtep@gmail.com>
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

#include "maemo5idle.h"

#include <mce/mode-names.h>
#include <mce/dbus-names.h>


Maemo5Idle::Maemo5Idle()
{
	mDbusInterface = new QDBusInterface(MCE_SERVICE, MCE_REQUEST_PATH,
					    MCE_REQUEST_IF, QDBusConnection::systemBus(),
					    this);
	display_off=false;
	QDBusConnection::systemBus().connect(MCE_SERVICE, MCE_SIGNAL_PATH, MCE_SIGNAL_IF,
					     MCE_DISPLAY_SIG, this,SLOT(displayStateChanged(const QDBusMessage &)));
	mDbusInterface->callWithCallback(MCE_DISPLAY_STATUS_GET, QList<QVariant>(), this, SLOT(setDisplayState(const QString &)));
	
	idle_timer = new QBasicTimer();
	idle_timer->start(60000,this);
	check_timer = new QTimer();
	connect(check_timer, SIGNAL(timeout()), SLOT(doCheck()));
	check_timer->start(60000); //every minute
}

void Maemo5Idle::doCheck()
{
	emit secondsIdle(idleMinutes*60);
}

void Maemo5Idle::setDisplayState(const QString &state)
{
	if (!state.isEmpty()) {
		if (state == MCE_DISPLAY_ON_STRING)
		{
			display_off=false;
			idle_timer->stop();
			idleMinutes=0;
		}
		else if (state == MCE_DISPLAY_OFF_STRING)
		{
			display_off=true;
			idle_timer->start(60000,this);
		}
	}
}

void Maemo5Idle::displayStateChanged(const QDBusMessage &message)
{
	QString state = message.arguments().at(0).toString();
	setDisplayState(state);
}

void Maemo5Idle::timerEvent(QTimerEvent* ev)
{
	idleMinutes+=1;
	ev->accept();
}


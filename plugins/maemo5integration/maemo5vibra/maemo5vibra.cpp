/****************************************************************************
**
** qutIM instant messenger
**
** Copyright (C) 2011 Evgeniy Degtyarev <degtep@gmail.com>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 2 of the License, or
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

#include <maemo5vibra.h>
#include <qutim/debug.h>
#include <QtDBus>

#include <mce/mode-names.h>
#include <mce/dbus-names.h>


using namespace qutim_sdk_0_3;


Maemo5Vibra::Maemo5Vibra() :	NotificationBackend("Vibration")
{
	setDescription(QT_TR_NOOP("Maemo 5 Vibration Notifications"));

	enableVibration();
}



void Maemo5Vibra::handleNotification(qutim_sdk_0_3::Notification *notification)
{
	ref(notification);
	vibrate(50);

	Q_UNUSED(notification);
}


Maemo5Vibra::~Maemo5Vibra()
{
	mDbusInterface->call(MCE_DEACTIVATE_VIBRATOR_PATTERN, "PatternChatAndEmail");
}

void Maemo5Vibra::enableVibration()
{
	mDbusInterface = new QDBusInterface(MCE_SERVICE, MCE_REQUEST_PATH,
										MCE_REQUEST_IF, QDBusConnection::systemBus(),
										this);
	mDbusInterface->call(MCE_ENABLE_VIBRATOR);
	display_off=false;
	QDBusConnection::systemBus().connect(MCE_SERVICE, MCE_SIGNAL_PATH, MCE_SIGNAL_IF,
										 MCE_DISPLAY_SIG, this,SLOT(displayStateChanged(const QDBusMessage &)));
	mDbusInterface->callWithCallback(MCE_DISPLAY_STATUS_GET, QList<QVariant>(), this, SLOT(setDisplayState(const QString &)));
}

void Maemo5Vibra::vibrate(int aTimeout)
{
    if (display_off)
    {
		mDbusInterface->call(MCE_ACTIVATE_VIBRATOR_PATTERN, "PatternChatAndEmail");
		QTimer::singleShot(aTimeout,this,SLOT(stopVibration()));
    }
}


void Maemo5Vibra::stopVibration()
{
	mDbusInterface->call(MCE_DEACTIVATE_VIBRATOR_PATTERN, "PatternChatAndEmail");
}

void Maemo5Vibra::displayStateChanged(const QDBusMessage &message)
{
	QString state = message.arguments().at(0).toString();
	setDisplayState(state);
}

void Maemo5Vibra::setDisplayState(const QString &state)
{
	if (!state.isEmpty()) {
		if (state == MCE_DISPLAY_ON_STRING)
		{
			display_off=false;
		}
		else if (state == MCE_DISPLAY_OFF_STRING)
		{
			display_off=true;
		}
	}
}

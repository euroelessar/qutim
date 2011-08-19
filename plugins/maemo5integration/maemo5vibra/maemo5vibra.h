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

#ifndef MAEMO5VIBRA_H
#define MAEMO5VIBRA_H

#include <qutim/notification.h>
#include <QtDBus>

class Maemo5Vibra : public QObject, public qutim_sdk_0_3::NotificationBackend
{
	Q_OBJECT
	Q_CLASSINFO("Service", "Vibration")
public:
	Maemo5Vibra();
	virtual ~Maemo5Vibra();
	virtual void handleNotification(qutim_sdk_0_3::Notification *notification);
protected slots:
	void setDisplayState(const QString &state);
	void displayStateChanged(const QDBusMessage &message);
	void stopVibration();

private:
	void enableVibration();
	void vibrate(int);

	QDBusInterface *mDbusInterface;
	bool display_off;
	bool vibrateWhenDisplayOn;
	int vibrationTime;

};

#endif // MAEMO5VIBRA_H

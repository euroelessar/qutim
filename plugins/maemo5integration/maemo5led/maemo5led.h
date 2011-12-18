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

#ifndef MAEMO5LED_H
#define MAEMO5LED_H

#include <qutim/notification.h>
#include <QtDBus>

class Maemo5Led : public QObject, public qutim_sdk_0_3::NotificationBackend
{
	Q_OBJECT
	Q_CLASSINFO("Service", "Led")
public:
	Maemo5Led();
	virtual ~Maemo5Led();
	virtual void handleNotification(qutim_sdk_0_3::Notification *notification);
protected slots:
	void setDisplayState(const QString &state);
	void displayStateChanged(const QDBusMessage &message);
private:
	void enableLed();
	QDBusInterface *mDbusInterface;
	bool display_off;
	bool showLedWhenDisplayOn;
	QString ledPattern;
};

#endif // MAEMO5LED_H


/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Evgeniy Degtyarev <degtep@gmail.com>
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

#ifndef MAEMO5IDLE_H
#define MAEMO5IDLE_H

#include <QObject>
#include <QTimer>
#include <QBasicTimer>
#include <QtDBus>

class Maemo5Idle : public QObject
{
	Q_OBJECT
	Q_CLASSINFO("Service", "Idle")
public:
	Maemo5Idle();



signals:
	void secondsIdle(int);

private slots:
	void doCheck();
	void setDisplayState(const QString &state);
	void displayStateChanged(const QDBusMessage &message);
	virtual void timerEvent(QTimerEvent* ev);

private:
	QDBusInterface *mDbusInterface;
	bool display_off;
	QBasicTimer *idle_timer;
	QTimer *check_timer;
	int idleMinutes;
};


#endif


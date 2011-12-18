/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2003 Justin Karneges
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

#ifndef IDLE_H
#define IDLE_H

#include <QObject>
#include <QCursor>
#include <QDateTime>
#include <QTimer>

namespace Psi
{
class IdlePlatform;

class Idle : public QObject
{
	Q_OBJECT
	Q_CLASSINFO("Service", "Idle")
public:
	Idle();
	~Idle();

	bool isActive() const;
	bool usingPlatform() const;
	void start();
	void stop();
	int secondsIdle();

signals:
	void secondsIdle(int);

private slots:
	void doCheck();

private:
	class Private;
	Private *d;
};

class IdlePlatform
{
public:
	IdlePlatform();
	~IdlePlatform();

	bool init();
	int secondsIdle();

private:
	class Private;
	Private *d;
};

class Idle::Private
{
public:
	Private() {}

	QPoint lastMousePos;
	QDateTime idleSince;

	bool active;
	int idleTime;
	QDateTime startTime;
	QTimer checkTimer;
};
}

#endif


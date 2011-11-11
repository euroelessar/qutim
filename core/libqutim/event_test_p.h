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

#ifndef EVENT_TEST_P_H
#define EVENT_TEST_P_H

#include "event.h"
#include <QEvent>

namespace qutim_sdk_0_3
{
	class LIBQUTIM_EXPORT EventTest : public QObject
	{
		Q_OBJECT
	public:
		inline EventTest(QObject *parent) : QObject(parent) {}
		void testE(Event &event);
		virtual bool eventFilter(QObject *, QEvent *);
	public slots:
		void test();
	signals:
	#ifndef Q_MOC_RUN
	public:
	#endif
		void signal();
	};

	LIBQUTIM_EXPORT void testEventSystemSpeed();
}

class QutimEvent : public QEvent
{
public:
	QutimEvent() : QEvent(QEvent::User) {}
};

#endif // EVENT_TEST_P_H


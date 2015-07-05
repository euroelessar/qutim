/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#ifndef MACIDLE_H
#define MACIDLE_H

#include <QObject>
#include <qutim/protocol.h>

namespace MacIntegration
{
using namespace qutim_sdk_0_3;

class MacIdlePrivate;
class MacIdle : public QObject
{
	Q_OBJECT
	Q_CLASSINFO("Service", "AutoAway")
	Q_DECLARE_PRIVATE(MacIdle)
public:
	enum Reason {
		Screensaver,
		Away,
		Inactive,
		NoIdle
	};
	MacIdle();
	~MacIdle();
	void setIdleOn(Reason reason);
	void setIdleOff();
	void reloadSettings();
private slots:
	void onTimeout();
private:
	QScopedPointer<MacIdlePrivate> d_ptr;
};
}

#endif // MACIDLE_H


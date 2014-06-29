/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ivan Vizir <define-true-false@yandex.com>
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

#include <QObject>
#include "../../apilayer/src/ApiProgressIndicatorStates.h"

class QWidget;

class ProgressIndicator : public QObject
{
	Q_OBJECT

	QWidget *m_window;

public:
	ProgressIndicator(QWidget *, QObject *parent = 0);
	void changeWindow(QWidget*);
	void setState(ProgressStates);
	void setValue(unsigned, unsigned max = 100);
	static void setState(QWidget *, ProgressStates);
	static void setValue(QWidget *, unsigned, unsigned max = 100);
	void clear();
};


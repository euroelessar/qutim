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

#include "ProgressIndicator.h"
#include "../../apilayer/src/ApiProgressIndicator.h"
#include <QWidget>

ProgressIndicator::ProgressIndicator(QWidget *w, QObject *parent)
	: QObject(parent)
{
	m_window = w;
}

void ProgressIndicator::changeWindow(QWidget* w)
{
	m_window = w;
}

void ProgressIndicator::setState(ProgressStates state)
{
	SetProgressState((HWND)m_window->winId(), state);
}

void ProgressIndicator::setValue(unsigned val, unsigned max)
{
	SetProgressValEx((HWND)m_window->winId(), val, max);
}

void ProgressIndicator::clear()
{
	setState(PS_None);
}

void ProgressIndicator::setValue(QWidget *w, unsigned val, unsigned max)
{
	ProgressIndicator i(w);
	i.setValue(val, max);
}

void ProgressIndicator::setState(QWidget *w, ProgressStates state)
{
	ProgressIndicator i(w);
	i.setState(state);
}


/****************************************************************************
**
** qutIM instant messenger
**
** Copyright (C) 2011 Aleksey Sidorov <sauron@citadelspb.com>
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
** along with this program. If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#ifndef FRAMEBUTTON_H
#define FRAMEBUTTON_H

#include <QPushButton>

class FrameButtonPrivate;
class FrameButton : public QPushButton
{
	 Q_OBJECT
	Q_DECLARE_PRIVATE(FrameButton)
public:
	explicit FrameButton(QWidget *parent = 0);
	virtual ~FrameButton();
protected:
	void enterEvent(QEvent *);
	void leaveEvent(QEvent *);
private:
	QScopedPointer<FrameButtonPrivate> d_ptr;
};

#endif // FRAMEBUTTON_H

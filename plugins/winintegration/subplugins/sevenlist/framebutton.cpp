/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <sauron@citadelspb.com>
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

#include "framebutton.h"
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>
#include <QMessageBox>
#include <QFile>

class FrameButtonPrivate
{
public:
	//QGraphicsDropShadowEffect effect;
};

FrameButton::FrameButton(QWidget *parent) :
	QPushButton(parent), d_ptr(new FrameButtonPrivate)
{
	Q_D(FrameButton);
	//d->effect.setColor(QColor(255, 50, 50));
	//d->effect.setBlurRadius(22);
	//d->effect.setOffset(1);
	//d->effect.setEnabled(false);
	//setGraphicsEffect(&d_func()->effect);
	setAutoFillBackground(false);

	QFile file(":/sevenlist/pushbutton.css");
	if (file.open(QIODevice::ReadOnly))
		setStyleSheet(file.readAll());

	setMaximumHeight(22);
	setMinimumSize(50, 22);

	//setStyleSheet(("QPushButton { "
	//			   "border: 3px;"
	//			   "border-image: url(:/sevenlist/redbutton.png);"
	//			   "}"));

	//setIconSize(QSize(12,12));
}

FrameButton::~FrameButton()
{

}

void FrameButton::enterEvent(QEvent *e)
{
	QPushButton::enterEvent(e);
	//d_func()->effect.setEnabled(true);
}

void FrameButton::leaveEvent(QEvent *e)
{
	QPushButton::leaveEvent(e);
	//d_func()->effect.setEnabled(false);
}




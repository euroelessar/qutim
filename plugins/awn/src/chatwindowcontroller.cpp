/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Stanislav (proDOOMman) Kosolapov <prodoomman@shell.tor.hu>
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

#include "chatwindowcontroller.h"
#include <QEvent>
#include <QWidget>

ChatWindowController::ChatWindowController(QObject *parent) :
    QObject(parent)
{
}

bool ChatWindowController::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type()==QEvent::Close || event->type()==QEvent::Hide)
    {
        event->ignore();
        (static_cast<QWidget*>(obj))->setWindowState(Qt::WindowMinimized);
        return true;
    }
    return QObject::eventFilter(obj,event);
}


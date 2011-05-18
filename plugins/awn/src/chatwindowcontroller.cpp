/*

    Copyright (c) 2010 by Stanislav (proDOOMman) Kosolapov <prodoomman@shell.tor.hu>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

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

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

#ifndef CHATWINDOWCONTROLLER_H
#define CHATWINDOWCONTROLLER_H

#include <QObject>

class ChatWindowController : public QObject
{
    Q_OBJECT
public:
    explicit ChatWindowController(QObject *parent = 0);
protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // CHATWINDOWCONTROLLER_H

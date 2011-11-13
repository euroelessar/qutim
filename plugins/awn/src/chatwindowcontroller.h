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


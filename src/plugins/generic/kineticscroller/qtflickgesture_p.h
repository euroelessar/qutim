/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Nokia Corporation and/or its subsidiary(-ies)
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

#ifndef QTFLICKGESTURE_P_H
#define QTFLICKGESTURE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qobject.h"
#include "qpointer.h"
#include "qevent.h"
#include "qgesture.h"
#include "qgesturerecognizer.h"
#include "qtscroller.h"

#ifndef QT_NO_GESTURES

class PressDelayHandler;

class QtFlickGesture : public QGesture
{
    Q_OBJECT

public:
    QtFlickGesture(QObject *receiver, Qt::MouseButton button, QObject *parent = 0);
    ~QtFlickGesture();

protected:
    bool eventFilter(QObject *o, QEvent *e);

private:
	QPointer<QObject> receiver;
    QtScroller *receiverScroller;
    Qt::MouseButton button; // NoButton == Touch
    bool macIgnoreWheel;
    // QWidget::mapFromGlobal is very expensive on X11, so we cache the global position of the widget
	QPointer<QWidget> receiverWindow;
    QPoint receiverWindowPos;

    static PressDelayHandler *pressDelayHandler;

    friend class QtFlickGestureRecognizer;
};

class QtFlickGestureRecognizer : public QGestureRecognizer
{
public:
    QtFlickGestureRecognizer(Qt::MouseButton button);

    QGesture *create(QObject *target);
    QGestureRecognizer::Result recognize(QGesture *state, QObject *watched, QEvent *event);
    void reset(QGesture *state);

private:
    Qt::MouseButton button; // NoButton == Touch
};

#endif // QT_NO_GESTURES

#endif // QTFLICKGESTURE_P_H


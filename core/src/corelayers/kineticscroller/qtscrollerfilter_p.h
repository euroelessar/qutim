/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QTSCROLLERFILTER_P_H
#define QTSCROLLERFILTER_P_H

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

#include <QObject>
#include <QItemSelection>
#include <QMap>
#include <QPoint>
#include "qtscroller.h"

class QAbstractScrollArea;
class QAbstractItemView;
class QWebView;
class QWebFrame;

class QtScrollerFilter : public QObject {
    Q_OBJECT

public:
    static QtScrollerFilter *instance();

    void add(QObject *);
    void remove(QObject *);

protected:
    QtScrollerFilter();

    bool eventFilter(QObject *o, QEvent *e);

#ifndef QTSCROLLER_NO_WEBKIT
    //+++ QWebView
    bool eventFilter_QWebView(QWebView *web, QEvent *event);
    QWebFrame *scrollingFrameAt_QWebView(QWebView *web, const QPoint &pos) const;
    //WebCore::VisibleSelection oldWebSelection;
    QMap<QWebView *, QWebFrame *> scrollingFrames;
#endif

    //+++ QAbstractScrollArea
    bool eventFilter_QAbstractScrollArea(QAbstractScrollArea *area, QEvent *event);
    bool canStartScrollingAt_QAbstractScrollArea(QAbstractScrollArea *area, const QPoint &pos) const;
    QMap<QAbstractScrollArea *, QPoint> overshoot;
    bool ignoreMove;

    //+++ QAbstractItemView
    bool eventFilter_QAbstractItemView(QAbstractItemView *view, QEvent *event);
    void stateChanged_QAbstractItemView(QAbstractItemView *view, QtScroller::State state);

    // the selection before the last mouse down. In case we have to restore it for scrolling    
    QItemSelection oldSelection; 
    QModelIndex oldCurrent;

protected slots:
    void stateChanged(QtScroller::State state);
    
private:
    static QtScrollerFilter *inst;
};

#endif // QTSCROLLERFILTER_P_H


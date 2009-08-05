/********************************************************************************************************
* PROGRAM : QSkinObject
* DATE - TIME : Samstag 30 Dezember 2006 - 12h04
* AUTHOR : Markus Künkle( http://amobos.org )
* FILENAME : qskinwidgetresizehandler.h
* LICENSE : GPL
* COMMENTARY : QObject subclass for theming Qt\'s QWidget.
********************************************************************************************************/


#ifndef QSKINWIDGETRESIZEHANDLER_P_H
#define QSKINWIDGETRESIZEHANDLER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  This header file may
// change from version to version without notice, or even be
// removed.
//
// We mean it.
//

#include <QObject>
#include <QPoint>
#include "qskinobject.h"
class QSkinObject;
class QMouseEvent;
class QKeyEvent;

class QSkinWidgetResizeHandler : public QObject
{
    Q_OBJECT
    friend class QSkinObject;
public:
    enum Action {
        Move        = 0x01,
        Resize        = 0x02,
        Any        = Move|Resize
    };

    explicit QSkinWidgetResizeHandler(QSkinObject * skobj, QWidget *parent, QWidget *cw = 0);
    void setActive(bool b) { setActive(Any, b); }
    void setActive(Action ac, bool b);
    bool isActive() const { return isActive(Any); }
    bool isActive(Action ac) const;
    void setMovingEnabled(bool b) { movingEnabled = b; }
    bool isMovingEnabled() const { return movingEnabled; }

    bool isButtonDown() const { return buttonDown; }

    void setExtraHeight(int h) { extrahei = h; }
    void setSizeProtection(bool b) { sizeprotect = b; }

    void setFrameWidth(int w) { fw = w; }

    void doResize();
    void doMove();

Q_SIGNALS:
    void activate();

protected:
    bool eventFilter(QObject *o, QEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void keyPressEvent(QKeyEvent *e);

private:
    Q_DISABLE_COPY(QSkinWidgetResizeHandler)
	QSkinObject *obj;
    enum MousePosition {
        Nowhere,
        TopLeft, BottomRight, BottomLeft, TopRight,
        Top, Bottom, Left, Right,
        Center
    };

    QWidget *widget;
    QWidget *childWidget;
    QPoint moveOffset;
    QPoint invertedMoveOffset;
    MousePosition mode;
    int fw;
    int extrahei;
    int range;
    uint buttonDown            :1;
    uint moveResizeMode            :1;
    uint activeForResize    :1;
    uint sizeprotect            :1;
    uint movingEnabled                    :1;
    uint activeForMove            :1;
	uint widgetType;
    void setMouseCursor(MousePosition m);
    bool isMove() const {
        return moveResizeMode && mode == Center;
    }
    bool isResize() const {
        return moveResizeMode && !isMove();
    }
};


#endif // QSkinWidgetResizeHandler_P_H

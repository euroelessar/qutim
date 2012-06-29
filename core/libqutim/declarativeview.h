#ifndef QUTIM_SDK_0_3_DECLARATIVEVIEW_H
#define QUTIM_SDK_0_3_DECLARATIVEVIEW_H
#include <QGraphicsView>
#include "libqutim_global.h"

class  QDeclarativeEngine;

namespace qutim_sdk_0_3 {

class LIBQUTIM_EXPORT DeclarativeView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit DeclarativeView(QWidget *parent = 0);

    static void setEngine(QDeclarativeEngine *engine);
    static QDeclarativeEngine *engine();
};

} // namespace qutim_sdk_0_3

#endif // QUTIM_SDK_0_3_DECLARATIVEVIEW_H

#include "declarativeview.h"
#include <QPointer>
#include <QDeclarativeEngine>

namespace qutim_sdk_0_3 {

static QPointer<QDeclarativeEngine> m_engine;

DeclarativeView::DeclarativeView(QWidget *parent) :
    QGraphicsView(parent)
{
}

void DeclarativeView::setEngine(QDeclarativeEngine *engine)
{
    Q_ASSERT(!m_engine);
    m_engine = engine;
}

QDeclarativeEngine *DeclarativeView::engine()
{
    if (!m_engine) {
        m_engine = new QDeclarativeEngine();
    }
    return m_engine;
}

} // namespace qutim_sdk_0_3

#include "selectablemousearea.h"

namespace QuickChat {

SelectableMouseArea::SelectableMouseArea(QQuickItem *parent) :
    QQuickItem(parent)
{
    setAcceptedMouseButtons(Qt::LeftButton);
    setFiltersChildMouseEvents(true);
}

void SelectableMouseArea::mousePressEvent(QMouseEvent *event)
{
    if (event->modifiers() & Qt::ShiftModifier)
        continueSelection(event);
    else
        startSelection(event);
}

void SelectableMouseArea::mouseMoveEvent(QMouseEvent *event)
{
    continueSelection(event);
}

void SelectableMouseArea::mouseReleaseEvent(QMouseEvent *event)
{
    for (ItemInfo &info : m_items) {
        if (info.moved)
            return;
    }

    QString link = linkAt(event->x(), event->y());
    if (!link.isEmpty())
        emit linkActivated(link);
}

void SelectableMouseArea::startSelection(QMouseEvent *event)
{
    for (ItemInfo &info : m_items) {
        int position = 0;
        QPointF pos = mapToItem(info.item, event->pos());
        info.deselect.invoke(info.item);
        info.positionAt.invoke(info.item, Q_RETURN_ARG(int, position), Q_ARG(qreal, pos.x()), Q_ARG(qreal, pos.y()));
        info.startPosition = position;
        info.lastPosition = position;
        info.moved = false;
    }
}

void SelectableMouseArea::continueSelection(QMouseEvent *event)
{
    for (ItemInfo &info : m_items) {
        int position = 0;
        QPointF pos = mapToItem(info.item, event->pos());
        info.positionAt.invoke(info.item, Q_RETURN_ARG(int, position), Q_ARG(qreal, pos.x()), Q_ARG(qreal, pos.y()));
        if (info.lastPosition == position)
            continue;

        info.lastPosition = position;
        info.select.invoke(info.item, Q_ARG(int, info.startPosition), Q_ARG(int, position));
        info.moved = true;
    }
}

static QMetaMethod resolveMethod(QObject *object, const char *name)
{
    const QMetaObject *meta = object->metaObject();
    const int index = meta->indexOfMethod(name);
    if (index >= 0)
        return meta->method(index);
    return QMetaMethod();
}

void SelectableMouseArea::addItem(QQuickItem *item)
{
    Q_ASSERT(item);
    ItemInfo info;
    info.item = item;
    info.select = resolveMethod(item, "select(int,int)");
    info.deselect = resolveMethod(item, "deselect()");
    info.linkAt = resolveMethod(item, "linkAt(qreal,qreal)");
    info.positionAt = resolveMethod(item, "positionAt(qreal,qreal)");
    Q_ASSERT(!m_items.contains(info));
    m_items << info;
}

void SelectableMouseArea::removeItem(QQuickItem *item)
{
    Q_ASSERT(item);
    ItemInfo info;
    info.item = item;
    Q_ASSERT(m_items.count(info) == 1);
    m_items.removeOne(info);
}

QString SelectableMouseArea::linkAt(qreal x, qreal y)
{
    QPointF originalPos(x, y);
    for (ItemInfo &info : m_items) {
        QString link;
        QPointF pos = mapToItem(info.item, originalPos);
        info.linkAt.invoke(info.item, Q_RETURN_ARG(QString, link), Q_ARG(qreal, pos.x()), Q_ARG(qreal, pos.y()));
        if (!link.isEmpty())
            return link;
    }
    return QString();
}

} // namespace QuickChat

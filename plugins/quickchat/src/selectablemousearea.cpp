#include "selectablemousearea.h"
#include <QQmlProperty>
#include <QQuickTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QCursor>
#include <QQuickWindow>
#include <QClipboard>
#include <QGuiApplication>

namespace QuickChat {

SelectableMouseArea::SelectableMouseArea(QQuickItem *parent) :
    QQuickItem(parent)
{
    setAcceptedMouseButtons(Qt::LeftButton);
    setFiltersChildMouseEvents(true);
    setHoverEnabled(true);
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

    processClick(event);
}

void SelectableMouseArea::hoverEnterEvent(QHoverEvent *event)
{
    m_hoverActive = true;
    updateHoverInfo(event->posF());
}

void SelectableMouseArea::hoverMoveEvent(QHoverEvent *event)
{
    updateHoverInfo(event->posF());
}

void SelectableMouseArea::hoverLeaveEvent(QHoverEvent *event)
{
    Q_UNUSED(event);

    updateHoverInfo(QPointF());
    m_hoverActive = false;
    unsetCursor();
}

bool SelectableMouseArea::isHoverEnabled() const
{
    return m_hoverEnabled;
}

void SelectableMouseArea::setHoverEnabled(bool hoverEnabled)
{
    if (m_hoverEnabled == hoverEnabled)
        return;

    if (!hoverEnabled)
        updateHoverInfo(QPointF());

    setAcceptHoverEvents(hoverEnabled);

    m_hoverEnabled = hoverEnabled;
    emit hoverEnabledChanged(hoverEnabled);
}

void SelectableMouseArea::updateHoverInfo(const QPointF &originalPos)
{
    ItemInfo *hoveredInfo = originalPos.isNull() ? nullptr : infoAt(originalPos);
    QQuickItem *hoveredItem = hoveredInfo ? hoveredInfo->item : nullptr;

    if (m_hoveredItem != hoveredItem && m_hoveredItem) {
        auto it = std::find_if(m_items.begin(), m_items.end(), [this] (const ItemInfo &info) {
            return info.item == m_hoveredItem;
        });
        if (it != m_items.end() && it->highlighter)
            it->highlighter->setPosition(-1);
    }
    m_hoveredItem = hoveredItem;

    bool specified = false;
    if (hoveredInfo) {
        const auto &info = *hoveredInfo;
        QPointF pos = mapToItem(info.item, originalPos);
        auto layout = info.document->textDocument()->documentLayout();
        int position = layout->hitTest(pos, Qt::ExactHit);

        if (hoveredInfo->highlighter) {
            info.highlighter->setPosition(position);
        }
        if (position >= 0) {
            QString link;
            info.linkAt.invoke(info.item, Q_RETURN_ARG(QString, link), Q_ARG(qreal, pos.x()), Q_ARG(qreal, pos.y()));
            setCursor(link.isEmpty() ? Qt::IBeamCursor : Qt::PointingHandCursor);
            specified = true;
        }
    }
    if (!specified)
        unsetCursor();
}

void SelectableMouseArea::startSelection(QMouseEvent *event)
{
    for (ItemInfo &info : m_items) {
        int position = 0;
        QPointF pos = mapToItem(info.item, event->localPos());
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
        QPointF pos = mapToItem(info.item, event->localPos());
        info.positionAt.invoke(info.item, Q_RETURN_ARG(int, position), Q_ARG(qreal, pos.x()), Q_ARG(qreal, pos.y()));
        if (info.lastPosition == position)
            continue;

        info.lastPosition = position;
        info.select.invoke(info.item, Q_ARG(int, info.startPosition), Q_ARG(int, position));
        info.moved = true;
    }
}

void SelectableMouseArea::processClick(QMouseEvent *event)
{
    if (ItemInfo *info = infoAt(event->localPos())) {
        QPointF pos = mapToItem(info->item, event->localPos());
        if (info->click.isValid()) {
            info->click.invoke(info->item, Q_ARG(QVariant, pos.x()), Q_ARG(QVariant, pos.y()));
            return;
        }

        int position = -1;
        info->positionAt.invoke(info->item, Q_RETURN_ARG(int, position), Q_ARG(qreal, pos.x()), Q_ARG(qreal, pos.y()));

        QQuickTextDocument *doc = info->item->property("textDocument").value<QQuickTextDocument *>();
        QTextCursor cursor(doc->textDocument());
        cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, position);

        QString link;
        info->linkAt.invoke(info->item, Q_RETURN_ARG(QString, link), Q_ARG(qreal, pos.x()), Q_ARG(qreal, pos.y()));
        if (!link.isEmpty()) {
            emit linkActivated(link);
            return;
        }
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

static QMetaProperty resolveProperty(QObject *object, const char *name)
{
    const QMetaObject *meta = object->metaObject();
    const int index = meta->indexOfProperty(name);
    if (index >= 0)
        return meta->property(index);
    return QMetaProperty();
}

void SelectableMouseArea::addItem(QQuickItem *item)
{
    Q_ASSERT(item);
    QQmlProperty highlighter(item, QStringLiteral("highlighter"));
    ItemInfo info;
    info.item = item;
    info.highlighter = highlighter.read().value<AnchorsHighlighter *>();
    info.document = item->property("textDocument").value<QQuickTextDocument *>();
    info.select = resolveMethod(item, "select(int,int)");
    info.deselect = resolveMethod(item, "deselect()");
    info.linkAt = resolveMethod(item, "linkAt(qreal,qreal)");
    info.positionAt = resolveMethod(item, "positionAt(qreal,qreal)");
    info.positionToRectangle = resolveMethod(item, "positionToRectangle(int)");
    info.click = resolveMethod(item, "click(QVariant,QVariant)");
    info.selectedText = resolveProperty(item, "selectedText");
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

void SelectableMouseArea::copy()
{
    if (m_items.isEmpty())
        return;

    auto getPosition = [this] (QQuickItem *item) {
        QPointF firstPos(item->x(), item->y());
        return item->parentItem()->mapToItem(this, firstPos);
    };

    std::sort(m_items.begin(), m_items.end(), [this, getPosition] (const ItemInfo &first, const ItemInfo &second) {
        QPointF firstPos = getPosition(first.item);
        QPointF secondPos = getPosition(second.item);
        if (qFuzzyCompare(firstPos.y(), secondPos.y()))
            return firstPos.x() < secondPos.x();
        return firstPos.y() < secondPos.y();
    });
    qDebug() << "copy request";

    QString result;
    qreal previousY = getPosition(m_items.first().item).y();
    for (int i = 0; i < m_items.size(); ++i) {
        const ItemInfo &info = m_items[i];
        QString text = info.selectedText.read(info.item).toString();
        if (text.isEmpty())
            continue;
        qreal currentY = getPosition(info.item).y();
        if (!result.isEmpty())
            result += qFuzzyCompare(previousY, currentY) ? ' ' : '\n';
        result += text;
        previousY = currentY;
    }

    if (!result.isEmpty())
        QGuiApplication::clipboard()->setText(result);
}

void SelectableMouseArea::updateHover()
{
    if (!m_hoverActive)
        return;

    if (QQuickWindow *window = QQuickItem::window()) {
        QPointF scenePos = window->mapFromGlobal(QCursor::pos());
        QPointF localPos = mapFromScene(scenePos);
        updateHoverInfo(localPos);
    }
}

QString SelectableMouseArea::linkAt(qreal x, qreal y)
{
    QPointF originalPos(x, y);
    if (ItemInfo *info = infoAt(originalPos)) {
        QString link;
        QPointF pos = mapToItem(info->item, originalPos);
        info->linkAt.invoke(info->item, Q_RETURN_ARG(QString, link), Q_ARG(qreal, pos.x()), Q_ARG(qreal, pos.y()));
        if (!link.isEmpty())
            return link;
    }
    return QString();
}

QQuickItem *SelectableMouseArea::itemAt(qreal x, qreal y)
{
    if (ItemInfo *info = infoAt(QPointF(x, y)))
        return info->item;
    return nullptr;
}

SelectableMouseArea::ItemInfo *SelectableMouseArea::infoAt(const QPointF &originalPos)
{
    for (ItemInfo &info : m_items) {
        QPointF pos = mapToItem(info.item, originalPos);
        if (info.item->contains(pos))
            return &info;
    }
    return nullptr;
}

} // namespace QuickChat

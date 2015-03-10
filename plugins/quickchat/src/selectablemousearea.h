#ifndef QUICKCHAT_SELECTABLEMOUSEAREA_H
#define QUICKCHAT_SELECTABLEMOUSEAREA_H

#include <QQuickItem>
#include <QMetaMethod>
#include "anchorshighlighter.h"

namespace QuickChat {

class SelectableMouseArea : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(bool hoverEnabled READ isHoverEnabled WRITE setHoverEnabled NOTIFY hoverEnabledChanged)
public:
    explicit SelectableMouseArea(QQuickItem *parent = 0);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void hoverEnterEvent(QHoverEvent *event);
    void hoverMoveEvent(QHoverEvent *event);
    void hoverLeaveEvent(QHoverEvent *event);

    bool isHoverEnabled() const;
    void setHoverEnabled(bool hoverEnabled);

public slots:
    void addItem(QQuickItem *item);
    void removeItem(QQuickItem *item);
    void copy();
    void updateHover();
    QString linkAt(qreal x, qreal y);
    QQuickItem *itemAt(qreal x, qreal y);

signals:
    void linkActivated(const QString &link);
    void hoverEnabledChanged(bool hoverEnabled);

protected:
    void updateHoverInfo(const QPointF &originalPos);
    void startSelection(QMouseEvent *event);
    void continueSelection(QMouseEvent *event);
    void processClick(QMouseEvent *event);

private:
    struct ItemInfo
    {
        QQuickItem *item = nullptr;
        AnchorsHighlighter *highlighter = nullptr;
        QQuickTextDocument *document = nullptr;
        QMetaMethod select;
        QMetaMethod deselect;
        QMetaMethod linkAt;
        QMetaMethod positionAt;
        QMetaMethod positionToRectangle;
        QMetaMethod click;
        QMetaProperty selectedText;
        int startPosition = 0;
        int lastPosition = 0;
        bool moved = false;

        bool operator ==(const ItemInfo &other) const
        {
            return item == other.item;
        }
    };
    ItemInfo *infoAt(const QPointF &originalPos);

    QList<ItemInfo> m_items;
    QQuickItem *m_hoveredItem;
    bool m_hoverEnabled = false;
    bool m_hoverActive = false;
};

} // namespace QuickChat

#endif // QUICKCHAT_SELECTABLEMOUSEAREA_H

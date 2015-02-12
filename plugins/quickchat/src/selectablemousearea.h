#ifndef QUICKCHAT_SELECTABLEMOUSEAREA_H
#define QUICKCHAT_SELECTABLEMOUSEAREA_H

#include <QQuickItem>
#include <QMetaMethod>

namespace QuickChat {

class SelectableMouseArea : public QQuickItem
{
    Q_OBJECT
public:
    explicit SelectableMouseArea(QQuickItem *parent = 0);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

public slots:
    void addItem(QQuickItem *item);
    void removeItem(QQuickItem *item);
    void copy();
    QString linkAt(qreal x, qreal y);

signals:
    void linkActivated(const QString &link);

protected:
    void startSelection(QMouseEvent *event);
    void continueSelection(QMouseEvent *event);
    void processClick(QMouseEvent *event);

private:
    struct ItemInfo
    {
        QQuickItem *item = nullptr;
        QMetaMethod select;
        QMetaMethod deselect;
        QMetaMethod linkAt;
        QMetaMethod positionAt;
        QMetaMethod click;
        int startPosition = 0;
        int lastPosition = 0;
        bool moved = false;

        bool operator ==(const ItemInfo &other) const
        {
            return item == other.item;
        }
    };

    QList<ItemInfo> m_items;
};

} // namespace QuickChat

#endif // QUICKCHAT_SELECTABLEMOUSEAREA_H

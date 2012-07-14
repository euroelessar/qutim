#include "quickmaskeffect.h"
#include <QPainter>

namespace MeegoIntegration
{

QuickMaskEffect::QuickMaskEffect(QObject *parent) :
    QGraphicsEffect(parent)
{
}

QDeclarativeItem *QuickMaskEffect::mask() const
{
    return m_mask;
}

void QuickMaskEffect::setMask(QDeclarativeItem *mask)
{
    if (m_mask != mask) {
        m_mask = mask;
        update();
        emit maskChanged(mask);
    }
}

void QuickMaskEffect::draw(QPainter *painter)
{
    if (!m_mask) {
        drawSource(painter);
        return;
    }
    QPoint offset;
    const QPixmap pixmap = sourcePixmap(Qt::LogicalCoordinates, &offset, QGraphicsEffect::NoPad);
    if (pixmap.size() != m_buffer.size())
        m_buffer = pixmap;
    
    QPainter p(&m_buffer);
    
    p.setCompositionMode(QPainter::CompositionMode_Source);
    p.fillRect(0, 0, m_buffer.width(), m_buffer.height(), Qt::transparent);
    m_mask->paint(&p, 0, 0);
    
    p.setCompositionMode(QPainter::CompositionMode_SourceOut);
    p.drawPixmap(0, 0, pixmap);
    
    painter->drawPixmap(offset, m_buffer);
}

}

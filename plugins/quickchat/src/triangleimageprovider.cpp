#include "triangleimageprovider.h"
#include <QPainter>
#include <QImage>

namespace QuickChat {

TriangleImageProvider::TriangleImageProvider()
    : QQuickImageProvider(Image, ForceAsynchronousImageLoading)
{
}

QImage TriangleImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    Q_UNUSED(requestedSize);
    double realSize = id.toDouble();
    if (size)
        *size = QSize(realSize + 0.5, realSize + 0.5);

    QColor color = Qt::white;
    QImage image(int(realSize + 0.5), int(realSize + 0.5), QImage::Format_ARGB32_Premultiplied);
    QPainter painter(&image);

    painter.setPen(color);

    QPainterPath path(QPointF(0, 0));
    path.lineTo(QPointF(realSize, 0));
//    path.lineTo();

    return image;
}

} // namespace QuickChat

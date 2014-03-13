#include "quickimageprovider.h"
#include <qutim/icon.h>

namespace qutim_sdk_0_3 {

QuickImageProvider::QuickImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Pixmap)
{
}

QPixmap QuickImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    Q_UNUSED(size);
    int pos = id.lastIndexOf('/');
    QString iconName = id.right(id.length() - pos);
    Icon icon = Icon(iconName);
    if (size)
        *size = QSize(128, 128);
    
    int width = requestedSize.width() > 0 ? requestedSize.width() : size->width();
    return icon.pixmap(width);
}

} // namespace qutim_sdk_0_3

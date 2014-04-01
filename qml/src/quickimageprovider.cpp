#include "quickimageprovider.h"
#include <qutim/icon.h>

namespace qutim_sdk_0_3 {

QuickImageProvider::QuickImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Pixmap)
{
}

QPixmap QuickImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    QString iconName = id.section('/', 0, 0);

    bool validIconSize = false;
    int iconSize = id.section('/', 1, 1).toInt(&validIconSize);
    Icon icon = Icon(iconName);

    QSize finalSize;
    if (validIconSize)
        finalSize = QSize(iconSize, iconSize);
    else if (requestedSize.width() > 0)
        finalSize = requestedSize;
    else
        finalSize = QSize(128, 128);

    if (size)
        *size = finalSize;

    return icon.pixmap(finalSize);
}

} // namespace qutim_sdk_0_3

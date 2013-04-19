#include "desktopthemeimageprovider.h"

#include <QIcon>
#include <QImage>
#include <QDebug>

DesktopThemeImageProvider::DesktopThemeImageProvider()
    : QQuickImageProvider(Image, ForceAsynchronousImageLoading)
{
}

DesktopThemeImageProvider::~DesktopThemeImageProvider()
{
}

QImage DesktopThemeImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    Q_UNUSED(requestedSize);
    Q_UNUSED(size);
    int pos = id.lastIndexOf('/');
    QString iconName = id.right(id.length() - pos);
    int width = requestedSize.width();

    auto icon = QIcon::fromTheme(iconName);
    if (width == -1 && icon.availableSizes().count())
        width = icon.availableSizes().last().width();
    auto pixmap = icon.pixmap(width);
    return pixmap.toImage();
}

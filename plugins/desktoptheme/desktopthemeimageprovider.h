#ifndef DESKTOPTHEMEIMAGEPROVIDER_H
#define DESKTOPTHEMEIMAGEPROVIDER_H

#include <QQuickImageProvider>

class DesktopThemeImageProvider : public QQuickImageProvider
{
    Q_DISABLE_COPY(DesktopThemeImageProvider)
public:
    DesktopThemeImageProvider();
    ~DesktopThemeImageProvider();

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;
};

#endif // DESKTOPTHEMEIMAGEPROVIDER_H


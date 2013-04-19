#include "statusthemeimageprovider.h"
#include <QImageReader>
#include <QDebug>
#include <QCoreApplication>

enum Type {
    Invalid,
    Offline,
    Available,
    Away,
    ExtendedAway,
    Invisible,
    Busy,
    Unknown,
    Error
};

StatusThemeImageProvider::StatusThemeImageProvider()
    : QQuickImageProvider(Image, ForceAsynchronousImageLoading)
{
    const QString base = qApp->applicationDirPath() + "/share/status/";
    m_baseStatuses = {
        { Invalid, base + "user-invisible.svg" },
        { Offline, base + "user-offline.svg" },
        { Available, base + "user-online.svg" },
        { Away, base + "user-away.svg" },
        { ExtendedAway, base + "user-away-extended.svg" },
        { Invisible, base + "user-invisible.svg" },
        { Busy, base + "user-busy.svg" },
        { Unknown, base + "user-offline.svg" },
        { Error, base + "user-offline.svg" }
    };
    m_statuses = {
        { "chat", base + "user-online-chat.svg" }
    };
}

StatusThemeImageProvider::~StatusThemeImageProvider()
{
}

QImage StatusThemeImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    const QString status = id.section(QLatin1Char('/'), 1, 1);
    QString imagePath = m_statuses.value(status);

    if (imagePath.isEmpty()) {
        const QString baseStatus = id.section(QLatin1Char('/'), 0, 0);
        imagePath = m_baseStatuses.value(baseStatus.toInt());
    }

    if (imagePath.isEmpty())
        return QImage();

    QImage image;
    QImageReader reader(imagePath);
    reader.setScaledSize(requestedSize);
    reader.read(&image);
    *size = image.size();
//    qDebug() << id << requestedSize << image.size();
    return image;
}


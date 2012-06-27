#include "quickiconloader.h"
#include <QIcon>

namespace MeegoIntegration {

QuickIconLoader::QuickIconLoader()
{
}

QIcon QuickIconLoader::doLoadIcon(const QString &name)
{
    Q_UNUSED(name);
    return QIcon();
}

QMovie *QuickIconLoader::doLoadMovie(const QString &name)
{
    Q_UNUSED(name);
    return 0;
}

QString QuickIconLoader::doIconPath(const QString &name, uint iconSize)
{
    Q_UNUSED(name);
    Q_UNUSED(iconSize);
    return QString();
}

QString QuickIconLoader::doMoviePath(const QString &name, uint iconSize)
{
    Q_UNUSED(name);
    Q_UNUSED(iconSize);
    return QString();
}

} // namespace MeegoIntegration

/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#include "iconsloaderimpl.h"
#include <qutim/systeminfo.h>
#include <qutim/debug.h>
#include <QCoreApplication>
#include <QImageWriter>
#include <QBuffer>

#if defined(Q_OS_MAC) || defined(Q_OS_WIN)
#	define QUTIM_DEFAULT_ICON_THEME "qutim-default"
#else
#	define QUTIM_DEFAULT_ICON_THEME ""
#endif

namespace Core
{

IconLoaderImpl::IconLoaderImpl()
{
    QStringList themeSearchPaths = QIcon::themeSearchPaths();
    themeSearchPaths << QCoreApplication::applicationDirPath() + QStringLiteral("/icons");
    themeSearchPaths << (SystemInfo::getPath(SystemInfo::ShareDir) + QStringLiteral("/icons"));
    themeSearchPaths << (SystemInfo::getPath(SystemInfo::SystemShareDir) + QStringLiteral("/icons"));
    QIcon::setThemeSearchPaths(themeSearchPaths);

#if defined(Q_OS_MAC) || defined(Q_OS_WIN)
    QIcon::setThemeName(QStringLiteral("qutim-default"));
#endif
}

IconLoaderImpl::~IconLoaderImpl()
{
}

QIcon IconLoaderImpl::doLoadIcon(const QString &name)
{
    return QIcon::fromTheme(name);
}

QMovie *IconLoaderImpl::doLoadMovie(const QString &name)
{
	Q_UNUSED(name);
	return 0;
}

QString IconLoaderImpl::doIconPath(const QString &name, uint iconSize)
{
    QIcon icon = doLoadIcon(name);

    QByteArray data;
    QBuffer buffer(&data);
    buffer.open(QBuffer::WriteOnly);

    QPixmap pixmap = icon.pixmap(iconSize);
    QImage image = pixmap.toImage();

    QImageWriter writer(&buffer, "png");
    writer.write(image);

    buffer.close();

    return QStringLiteral("data:image/png;base64,") + QString::fromLatin1(data.toBase64());
}

QString IconLoaderImpl::doMoviePath(const QString &name, uint iconSize)
{
	Q_UNUSED(name);
	Q_UNUSED(iconSize);
	return QString();
}

}


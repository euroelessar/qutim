/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2013 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include <QApplication>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QFileInfo>

static QString adjustPath(const QString &path)
{
#if defined(Q_OS_MAC)
    if (!QDir::isAbsolutePath(path))
        return QStringLiteral("%1/../Resources/%2")
                .arg(QCoreApplication::applicationDirPath(), path);
#elif defined(Q_OS_QNX)
    if (!QDir::isAbsolutePath(path))
        return QStringLiteral("app/native/%1").arg(path);
#elif defined(Q_OS_UNIX) && !defined(Q_OS_ANDROID)
    const QString pathInInstallDir =
            QStringLiteral("%1/../%2").arg(QCoreApplication::applicationDirPath(), path);
    if (QFileInfo(pathInInstallDir).exists())
        return pathInInstallDir;
#endif
    return path;
}

static QUrl qmlFilePath(const QString &filePath)
{
    const QString adjustedPath = adjustPath(filePath);
#ifdef Q_OS_ANDROID
    return QUrl(QStringLiteral("assets:/") + adjustedPath);
#else
    return QUrl::fromLocalFile(adjustedPath);
#endif
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    int result;
    {
        QQmlEngine engine;
        engine.addImportPath(adjustPath(QStringLiteral("imports")));
        QQmlComponent component(&engine);
        component.loadUrl(qmlFilePath(QStringLiteral("qml/main.qml")));
        if (!component.isReady()) {
            qWarning("%s", qPrintable(component.errorString()));
            return -1;
        }
        QScopedPointer<QObject> topLevel(component.create());
        QObject::connect(&engine, SIGNAL(quit()), &app, SLOT(quit()));
        result = app.exec();
    }
    qDebug() << "all should be dead";

    return result;
}

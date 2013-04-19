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

//#include <QtGui/QGuiApplication>
#include <QApplication>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QQuickWindow>

#include <QStyleFactory>
//#include "qtquick2applicationviewer.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

//    QtQuick2ApplicationViewer viewer;
//    viewer.setMainQmlFile(QStringLiteral("qml/ureen/main.qml"));
//    viewer.showExpanded();

    int result;
    {
        QQmlEngine engine;
        engine.addImportPath(qApp->applicationDirPath() + "/imports");
        QQmlComponent component(&engine);
        component.loadUrl(QUrl::fromLocalFile(QStringLiteral("qml/main.qml")));
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

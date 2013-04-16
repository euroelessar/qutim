//#include <QtGui/QGuiApplication>
#include <QApplication>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QQuickWindow>
//#include "qtquick2applicationviewer.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

//    QtQuick2ApplicationViewer viewer;
//    viewer.setMainQmlFile(QStringLiteral("qml/ureen/main.qml"));
//    viewer.showExpanded();

    QQmlEngine engine;
    QQmlComponent component(&engine);
    component.loadUrl(QUrl::fromLocalFile(QStringLiteral("qml/ureen/main.qml")));
    if (!component.isReady()) {
        qWarning("%s", qPrintable(component.errorString()));
        return -1;
    }
    QObject *topLevel = component.create();
    QQuickWindow *window = qobject_cast<QQuickWindow *>(topLevel);
    if ( !window ) {
        qWarning("Error: Your root item has to be a Window.");
        return -1;
    }
    QObject::connect(&engine, SIGNAL(quit()), &app, SLOT(quit()));
    window->show();


    return app.exec();
}

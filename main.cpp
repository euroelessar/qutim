#include <QtCore/QDebug>
#include <QtCore/QTime>
#include <QtGui/QApplication>
#include <QtGui/QIcon>
#include <QtGui/QLabel>
#include "src/xdg.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    XdgIconManager manager;

    qDebug() << manager.themeNames() << manager.themeIds() << xdgGetGnomeTheme() << xdgGetKdeTheme();

    const XdgIconTheme *current = manager.defaultTheme();

    if(current) {
        qDebug() << current->getIconPath("document-new", 22);
        qDebug() << current->getIconPath("document-open-subtype-test", 64);
        QIcon icon = current->getIcon("document-save-test");
        QPixmap pixmap = icon.pixmap(64, QIcon::Selected);
        QLabel label;
        label.setPixmap(pixmap);
        label.show();
        return QApplication::exec();
    } else {
        qDebug() << "Current theme not found!";
    }
}

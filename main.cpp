#include <QtCore/QDebug>
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

    if(current)
    {
        qDebug() << current->getIconPath("document-new", 22);
        qDebug() << current->getIconPath("document-open", 22);

        QIcon icon = current->getIcon("document-save");
        QPixmap pixmap = icon.pixmap(64, QIcon::Selected);
        QLabel label;
        label.setPixmap(pixmap);
        label.show();
        return QApplication::exec();
    }
    else
    {
        qDebug() << "Current theme not found!";
    }
}

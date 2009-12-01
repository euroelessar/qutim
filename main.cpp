#include <QtCore/QDebug>
#include <QtGui/QApplication>
#include <QtGui/QIcon>
#include <QtGui/QLabel>
#include "src/xdg.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    XdgIconManager manager;
    XdgThemeChooserGnome2 gnomeChooser;
    XdgThemeChooserKde4 kdeChooser;

    qDebug() << manager.themeNames() << manager.themeIds() << gnomeChooser.getThemeId() << kdeChooser.getThemeId();

    const XdgIconTheme *current = manager.defaultTheme();

    if(current)
    {
        qDebug() << current->getIconPath("document-new", 22);
        qDebug() << current->getIconPath("document-open", 22);

        QPixmap pixmap = current->getPixmap("document-save", 28);
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

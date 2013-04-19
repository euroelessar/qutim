#ifndef DESKTOPTHEME_PLUGIN_H
#define DESKTOPTHEME_PLUGIN_H

#include <QQmlExtensionPlugin>

class DesktopThemePlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")
    
public:
    void initializeEngine(QQmlEngine *engine, const char *uri) override;
    void registerTypes(const char *uri) override;
};

#endif // DESKTOPTHEME_PLUGIN_H


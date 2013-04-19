#ifndef STATUSTHEME_PLUGIN_H
#define STATUSTHEME_PLUGIN_H

#include <QQmlExtensionPlugin>

class StatusthemePlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")
    
public:
    void initializeEngine(QQmlEngine *engine, const char *uri);
    void registerTypes(const char *uri);
};

#endif // STATUSTHEME_PLUGIN_H


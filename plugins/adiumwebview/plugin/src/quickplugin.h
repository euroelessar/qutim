#ifndef QUTIM_SDK_0_3_QUICKPLUGIN_H
#define QUTIM_SDK_0_3_QUICKPLUGIN_H

#include <QQmlExtensionPlugin>

namespace AdiumWebView {
class QuickPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")
public:
    QuickPlugin();
    
    void initializeEngine(QQmlEngine *engine, const char *uri);
    void registerTypes(const char *uri);
};
}

#endif // QUTIM_SDK_0_3_QUICKPLUGIN_H

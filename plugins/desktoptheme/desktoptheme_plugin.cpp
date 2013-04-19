#include "desktoptheme_plugin.h"
#include "desktopthemeimageprovider.h"

#include <qqml.h>

void DesktopThemePlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_UNUSED(uri);
    engine->addImageProvider("icontheme", new DesktopThemeImageProvider);
}

void DesktopThemePlugin::registerTypes(const char *uri)
{
    Q_UNUSED(uri);
    qmlRegisterBaseTypes(uri, 0, 4);
}

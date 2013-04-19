#include "statustheme_plugin.h"
#include "statusthemeimageprovider.h"

#include <qqml.h>
#include <QQmlEngine>


void StatusthemePlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_UNUSED(uri);
    engine->addImageProvider("statustheme", new StatusThemeImageProvider);
}

void StatusthemePlugin::registerTypes(const char *uri)
{
    Q_UNUSED(uri);
    qmlRegisterBaseTypes(uri, 0, 4);
}


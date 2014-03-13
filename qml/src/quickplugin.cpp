#include "quickplugin.h"
#include "quickconfig.h"
#include "quickimageprovider.h"
#include <qutim/notification.h>
#include <qqml.h>

namespace qutim_sdk_0_3 {

QuickPlugin::QuickPlugin()
{
}

void QuickPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_ASSERT(QLatin1String(uri) == QStringLiteral("org.qutim"));
    
    engine->addImageProvider("xdg", new QuickImageProvider);
}

void QuickPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(QLatin1String(uri) == QStringLiteral("org.qutim"));
    
    qmlRegisterUncreatableType<Notification>(
                "org.qutim", 0, 4,
                "Notification",
                QStringLiteral("Unable to create notification inside QtQuick"));
    qmlRegisterType<QuickConfig>("org.qutim", 0, 4, "Config");
}
}

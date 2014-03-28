#define QUTIM_PLUGIN_NAME "org.qutim"

#include "quickplugin.h"
#include "quickconfig.h"
#include "quickemoticons.h"
#include "quickemoticonstheme.h"
#include "quickimageprovider.h"
#include "quickservice.h"
#include "quickpluginwrapper.h"
#include <qutim/notification.h>
#include <qutim/chatsession.h>
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
    qmlRegisterType<QuickEmoticons>("org.qutim", 0, 4, "Emoticons");
    qmlRegisterType<QuickEmoticonsTheme>("org.qutim", 0, 4, "EmoticonsTheme");
    qmlRegisterType<QuickService>("org.qutim", 0, 4, "Service");
    qmlRegisterType<QuickPluginWrapper>("org.qutim", 0, 4, "Plugin");
    qmlRegisterUncreatableType<ChatUnit>("org.qutim", 0, 4, "ChatUnit", "ChatUnit is pure virtual class");
    qmlRegisterUncreatableType<ChatSession>("org.qutim", 0, 4, "ChatSession", "ChatSession is pure virtual class");
}
}

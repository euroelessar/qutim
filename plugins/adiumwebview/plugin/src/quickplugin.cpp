#define QUTIM_PLUGIN_NAME "org.qutim.adiumwebview"

#include "quickplugin.h"
#include <qqml.h>
#include "../../lib/webkitmessageviewstyle.h"
#include "messageviewcontroller.h"

namespace AdiumWebView {

QuickPlugin::QuickPlugin()
{
}

void QuickPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_UNUSED(engine);
    Q_ASSERT(QLatin1String(uri) == QStringLiteral("org.qutim.adiumwebview"));
}

void QuickPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(QLatin1String(uri) == QStringLiteral("org.qutim.adiumwebview"));

    qmlRegisterType<WebKitMessageViewStyle>("org.qutim.adiumwebview", 0, 4, "MessageViewStyle");
    qmlRegisterType<MessageViewController>("org.qutim.adiumwebview", 0, 4, "MessageViewController");
}
}

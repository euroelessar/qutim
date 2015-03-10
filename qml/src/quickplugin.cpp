#include "quickplugin.h"
#include "quickconfig.h"
#include "quickemoticons.h"
#include "quickemoticonstheme.h"
#include "quickimageprovider.h"
#include "quickavatarprovider.h"
#include "quickservice.h"
#include "quickpluginwrapper.h"
#include "quickactioncontainer.h"
#include "quickmenucontainer.h"
#include "quickactionextender.h"
#include "quickthememanager.h"
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
    engine->addImageProvider("avatar", new QuickAvatarProvider);
}

void QuickPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(QLatin1String(uri) == QStringLiteral("org.qutim"));
    
    qmlRegisterUncreatableType<Notification>(
                "org.qutim", 0, 4,
                "Notification",
                QStringLiteral("Unable to create notification inside QtQuick"));
    qmlRegisterUncreatableType<MenuController>(
                "org.qutim", 0, 4,
                "MenuController",
                QStringLiteral("Unable to create notification inside QtQuick"));
    qmlRegisterUncreatableType<MessageUnitData>(
                "org.qutim", 0, 4,
                "MessageUnitData",
                QStringLiteral("Unable to create notification inside QtQuick"));
    qmlRegisterUncreatableType<Message>(
                "org.qutim", 0, 4,
                "Message",
                QStringLiteral("Unable to create notification inside QtQuick"));
    qmlRegisterUncreatableType<Status>(
                "org.qutim", 0, 4,
                "Status",
                QStringLiteral("Unable to create notification inside QtQuick"));
    qmlRegisterType<QuickConfig>("org.qutim", 0, 4, "Config");
    qmlRegisterType<QuickEmoticons>("org.qutim", 0, 4, "Emoticons");
    qmlRegisterType<QuickEmoticonsTheme>("org.qutim", 0, 4, "EmoticonsTheme");
    qmlRegisterType<QuickService>("org.qutim", 0, 4, "Service");
    qmlRegisterType<QuickPluginWrapper>("org.qutim", 0, 4, "Plugin");
    qmlRegisterType<QuickAction>("org.qutim", 0, 4, "Action");
    qmlRegisterType<QuickActionContainer>("org.qutim", 0, 4, "ActionContainer");
    qmlRegisterType<QuickMenuContainer>("org.qutim", 0, 4, "MenuContainer");
    qmlRegisterType<QuickActionExtender>("org.qutim", 0, 4, "ActionListExtender");
    qmlRegisterType<QuickThemeManager>("org.qutim", 0, 4, "ThemeManager");
    qmlRegisterUncreatableType<ChatUnit>("org.qutim", 0, 4, "ChatUnit", "ChatUnit is pure virtual class");
    qmlRegisterUncreatableType<ChatSession>("org.qutim", 0, 4, "ChatSession", "ChatSession is pure virtual class");
    qmlRegisterUncreatableType<MenuController>("org.qutim", 0, 4, "MenuController", "MenuController is pure virtual class");
}
}

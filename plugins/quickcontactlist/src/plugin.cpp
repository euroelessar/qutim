#include "plugin.h"
#include <qutim/declarativeview.h>
#include <qutim/systeminfo.h>
#include <QQmlComponent>
#include <QFileSelector>

namespace QuickContactList {

using namespace qutim_sdk_0_3;

Plugin::Plugin()
{
}


void Plugin::init()
{
    setCapabilities(Loadable);
}

bool Plugin::load()
{
    QString path = SystemInfo::getPath(SystemInfo::SystemShareDir);
    path += QStringLiteral("/qml/quickcontactlist/main.qml");

    QFileSelector selector;
    QString selectedPath = selector.select(path);

    QQuickView *window = new DeclarativeView;
    window->setResizeMode(QQuickView::SizeRootObjectToView);
    window->setSource(QUrl::fromLocalFile(selectedPath));
    window->show();
    
    return true;
}

bool Plugin::unload()
{
    return false;
}

} // namespace QuickChat

QUTIM_EXPORT_PLUGIN(QuickContactList::Plugin)

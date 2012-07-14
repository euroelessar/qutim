#include "quickmenubuilder.h"
#include <qutim/thememanager.h>
#include <qutim/metaobjectbuilder.h>
#include "applicationwindow.h"
#include <qutim/servicemanager.h>
#include <QDeclarativeListReference>

namespace MeegoIntegration {

using namespace qutim_sdk_0_3;

QuickActionFilter::QuickActionFilter(QuickMenuBuilder *parent)
    : QObject(parent), m_builder(parent)
{
}

QuickActionFilter::~QuickActionFilter()
{
}

void QuickActionFilter::actionAdded(QAction *action, int index)
{
    QObject::connect(action, SIGNAL(changed()),
                     this, SLOT(onActionChanged()));
    m_actions.insert(index, action);
    if (action->isEnabled() && action->isVisible()) {
        int visibleIndex = 0;
        for (int i = 0; i < m_actions.size(); ++i) {
            QAction *tmp = m_actions.at(i);
            if (tmp == action)
                break;
            visibleIndex += (tmp->isVisible() && tmp->isEnabled());
        }
        m_visibleActions.insert(visibleIndex, action);
        m_builder->actionAdded(action, visibleIndex);
    }
}

void QuickActionFilter::actionRemoved(int index)
{
    QAction *action = m_actions.takeAt(index);
    int visibleIndex = m_visibleActions.indexOf(action);
    if (visibleIndex != -1) {
        m_visibleActions.removeAt(visibleIndex);
        m_builder->actionRemoved(visibleIndex);
    }
}

void QuickActionFilter::actionsCleared()
{
    m_actions.clear();
    m_visibleActions.clear();
    m_builder->actionsCleared();
}

void QuickActionFilter::onActionChanged()
{
    QAction *action = qobject_cast<QAction*>(sender());
    bool visible = action->isEnabled() && action->isVisible();
    bool previousVisible = m_visibleActions.contains(action);
    if (visible == previousVisible) {
        return;
    } else if (visible) {
        int index = 0;
        for (int i = 0; i < m_actions.size(); ++i) {
            QAction *tmp = m_actions.at(i);
            if (tmp == action)
                break;
            index += (tmp->isVisible() && tmp->isEnabled());
        }
        m_visibleActions.insert(index, action);
        m_builder->actionAdded(action, index);
    } else { // !visible
        int index = m_visibleActions.indexOf(action);
        m_visibleActions.removeAt(index);
        m_builder->actionRemoved(index);
    }
}

QuickMenuBuilder::QuickMenuBuilder(QObject *parent) :
    QObject(parent), m_menu(0)
{
    QString basePath = ThemeManager::path(QLatin1String("declarative"), QLatin1String("meego"));
    QUrl url = QUrl::fromLocalFile(basePath + "/ControlledMenuItem.qml");
    m_delegate = new QDeclarativeComponent(ApplicationWindow::instance()->engine(), url, this);
    qDebug() << url << m_delegate->isLoading() << m_delegate->isError() << m_delegate->errorString();
    m_container.addHandler(new QuickActionFilter(this));
    m_container.show();
}

QuickMenuBuilder::~QuickMenuBuilder()
{
    m_container.hide();
}

qutim_sdk_0_3::MenuController *QuickMenuBuilder::controller() const
{
    return m_controller;
}

void QuickMenuBuilder::setController(qutim_sdk_0_3::MenuController *controller)
{
    if (m_controller != controller) {
        m_controller = controller;
        emit controllerChanged(controller);
        m_container.setController(controller);
    }
}

QDeclarativeItem *QuickMenuBuilder::menu() const
{
    return m_menu;
}

void QuickMenuBuilder::setMenu(QDeclarativeItem *menu)
{
    if (m_menu != menu) {
        m_menu = menu;
        emit menuChanged(menu);
        foreach (QDeclarativeItem *item, m_items)
            setItemParent(item, menu);
    }
}

void QuickMenuBuilder::actionAdded(QAction *action, int index)
{
    QDeclarativeItem *item = static_cast<QDeclarativeItem*>(m_delegate->beginCreate(qmlContext(this)));
    item->setProperty("action", qVariantFromValue<QObject*>(action));
    setItemParent(item, m_menu);
    if (QDeclarativeItem *next = m_items.value(index))
        item->stackBefore(next);
    m_items.insert(index, item);
    m_delegate->completeCreate();
}

void QuickMenuBuilder::actionRemoved(int index)
{
    delete m_items.takeAt(index);
}

void QuickMenuBuilder::actionsCleared()
{
    QList<QDeclarativeItem *> items;
    qSwap(items, m_items);
    qDeleteAll(items);
}

void QuickMenuBuilder::setItemParent(QDeclarativeItem *item, QDeclarativeItem *menu)
{
    item->setParent(menu);
    item->setParentItem(menu);
}

} // namespace MeegoIntegration

#include "quickactioncontainer.h"
#include <QAction>

namespace qutim_sdk_0_3 {

QuickAction *create(QAction *action)
{
    QuickAction *result = new QuickAction(nullptr);

    auto onChanged = [result, action] () {
        result->setText(action->text());
        result->setIcon(action->icon());
        result->setTooltip(action->toolTip());
        result->setEnabled(action->isEnabled());
        result->setCheckable(action->isCheckable());
        result->setChecked(action->isChecked());
        result->setShortcut(action->shortcut());
        result->setVisible(action->isVisible());
    };

    QMetaObject::Connection connection = QObject::connect(action, &QAction::changed, onChanged);
    QObject::connect(result, &QObject::destroyed, [connection] () {
        QObject::disconnect(connection);
    });
    QObject::connect(result, &QuickAction::triggered, action, &QAction::trigger);

    onChanged();

    return result;
}

QuickActionContainer::QuickActionContainer(QObject *parent) :
    QObject(parent), m_visible(false), m_completed(false)
{
}

QuickActionContainer::~QuickActionContainer()
{
    for (auto action : m_actions)
        action->deleteLater();
}

void QuickActionContainer::handleActionAdded(int index, QAction *action)
{
    QuickAction *quickAction = create(action);
    m_actions.insert(index, quickAction);
    emit actionAdded(index, quickAction);
}

void QuickActionContainer::handleActionRemoved(int index)
{
    QuickAction *action = m_actions.takeAt(index);
    emit actionRemoved(index);
    action->deleteLater();
}

void QuickActionContainer::handleActionsCleared()
{
    for (int index = m_actions.size() - 1; index >= 0; --index)
        handleActionRemoved(index);
}

void QuickActionContainer::classBegin()
{
}

void QuickActionContainer::componentComplete()
{
    m_completed = true;

    if (m_container.size() > 0) {
        for (int index = 0; index < m_container.size(); ++index)
            handleActionAdded(index, m_container.action(index));
    }
    m_container.addHandler(this);
}

QObject *QuickActionContainer::controller() const
{
    return m_controller;
}

bool QuickActionContainer::visible() const
{
    return m_visible;
}

void QuickActionContainer::setController(QObject *controller)
{
    if (m_controller != controller) {
        m_controller = controller;
        m_container.setController(qobject_cast<MenuController *>(controller));
        emit controllerChanged(controller);
    }
}

void QuickActionContainer::setVisible(bool visible)
{
    if (m_visible != visible) {
        m_visible = visible;

        if (m_visible)
            m_container.show();
        else
            m_container.hide();

        emit visibleChanged(visible);
    }
}

int QuickActionContainer::count() const
{
    return m_actions.size();
}

QuickAction *QuickActionContainer::item(int index)
{
    return m_actions.at(index);
}

} // namespace qutim_sdk_0_3

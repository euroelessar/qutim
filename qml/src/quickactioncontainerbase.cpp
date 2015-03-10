#include "quickactioncontainerbase.h"

namespace qutim_sdk_0_3 {

QuickActionContainerBase::QuickActionContainerBase(QObject *parent) :
    QObject(parent), m_controller(nullptr), m_visible(false), m_completed(false)
{
}

QuickActionContainerBase::~QuickActionContainerBase()
{
}

void QuickActionContainerBase::classBegin()
{
}

void QuickActionContainerBase::componentComplete()
{
    m_completed = true;

    if (m_container.size() > 0) {
        for (int index = 0; index < m_container.size(); ++index)
            handleActionAdded(index, m_container.action(index));
    }
    m_container.addHandler(this);
}

QObject *QuickActionContainerBase::controller() const
{
    return m_controller;
}

bool QuickActionContainerBase::visible() const
{
    return m_visible;
}

void QuickActionContainerBase::setController(QObject *controller)
{
    if (m_controller != controller) {
        if (m_controller) {
            disconnect(m_controller, nullptr, this, nullptr);
        }

        if (controller) {
            connect(controller, &QObject::destroyed, this, [this] () {
                m_controller = nullptr;
                m_container.setController(nullptr);
                emit controllerChanged(nullptr);
            });
        }

        m_controller = controller;
        m_container.setController(qobject_cast<MenuController *>(controller));
        emit controllerChanged(controller);
    }
}

void QuickActionContainerBase::setVisible(bool visible)
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

QuickActionContainerBase::ActionType QuickActionContainerBase::actionType() const
{
    return m_actionType;
}

void QuickActionContainerBase::setActionType(QuickActionContainerBase::ActionType actionType)
{
    if (m_actionType == actionType)
        return;

    if (actionType == AllActions) {
        m_container.setFilter(ActionContainer::Invalid, QVariant());
    } else {
        m_container.setFilter(ActionContainer::TypeMatch, QVariant(int(actionType)));
    }

    m_actionType = actionType;
    emit actionTypeChanged(actionType);
}

QuickAction *QuickActionContainerBase::create(QAction *action)
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
        result->setSeparator(action->isSeparator());
    };

    QObject::connect(action, &QAction::changed, result, onChanged);
    QObject::connect(result, &QuickAction::triggered, action, &QAction::trigger);

    onChanged();

    return result;
}

QuickAction *QuickActionContainerBase::createSeparator()
{
    QuickAction *result = new QuickAction(nullptr);
    result->setSeparator(true);
    return result;
}

} // namespace qutim_sdk_0_3

#include "quickactioncontainer.h"
#include <QAction>
#include <QCoreApplication>

namespace qutim_sdk_0_3 {

QuickActionContainer::QuickActionContainer(QObject *parent) :
    QuickActionContainerBase(parent)
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
    emit countChanged(m_actions.size());
}

void QuickActionContainer::handleActionRemoved(int index)
{
    QuickAction *action = m_actions.takeAt(index);
    emit actionRemoved(index);
    emit countChanged(m_actions.size());
    action->deleteLater();
}

void QuickActionContainer::handleActionsCleared()
{
    for (int index = m_actions.size() - 1; index >= 0; --index)
        handleActionRemoved(index);
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

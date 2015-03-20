#include "quickmenucontainer.h"

namespace qutim_sdk_0_3 {

QuickMenuContainer::QuickMenuContainer(QObject *parent) :
    QuickActionContainerBase(parent)
{
}

QuickMenuContainer::~QuickMenuContainer()
{
    while (!m_entries.isEmpty())
        delete m_entries.takeLast().action;
}

void QuickMenuContainer::handleActionAdded(int originalIndex, QAction *action)
{
    int index = mapIndex(originalIndex);
    auto generator = ActionGenerator::get(action);
    const int type = generator->type();

    QuickAction *quickAction = create(action);

    Entry entry = {
        quickAction,
        type,
        false
    };

    insertAction(index, entry);
    validateAllSeparators();
}

void QuickMenuContainer::handleActionRemoved(int originalIndex)
{
    int index = mapIndex(originalIndex);

    removeAction(index);
    validateAllSeparators();
}

void QuickMenuContainer::handleActionsCleared()
{
    for (int i = m_entries.size() - 1; i >= 0; --i)
        removeAction(i);
}

int QuickMenuContainer::mapIndex(int originalIndex)
{
    int index = 0;
    for (int i = 0; i < originalIndex; ++index) {
        const Entry &entry = m_entries[index];
        if (entry.separator)
            continue;

        ++i;
    }
    return index;
}

void QuickMenuContainer::validateAllSeparators()
{
    // Remove separators at start
    while (m_entries.size() > 0 && m_entries.first().separator)
        removeAction(0);

    // Remove separators at end
    while (m_entries.size() > 0 && m_entries.last().separator)
        removeAction(m_entries.size() - 1);

    // Remove all duplicates
    for (int i = 0; i + 1 < m_entries.size(); ++i) {
        const Entry &cur = m_entries.at(i);
        const Entry &next = m_entries.at(i + 1);

        if (cur.separator && next.separator) {
            removeAction(i--);
            continue;
        }
    }

    // Remove old separators
    for (int i = 1; i < m_entries.size() - 1; ++i) {
        const Entry &prev = m_entries.at(i - 1);
        const Entry &cur = m_entries.at(i);
        const Entry &next = m_entries.at(i + 1);

        if (cur.separator && prev.type == next.type) {
            removeAction(i--);
            continue;
        }
    }

    // Insert missed separators
    for (int i = 0; i < m_entries.size() - 1; ++i) {
        const Entry &cur = m_entries.at(i);
        const Entry &next = m_entries.at(i + 1);

        if (!cur.separator && !next.separator && cur.type != next.type)
            insertSeparator(i + 1);
    }
}

void QuickMenuContainer::insertSeparator(int index)
{
    Entry entry = {
        createSeparator(),
        0,
        true
    };

    insertAction(index, entry);
}

void QuickMenuContainer::insertAction(int index, const Entry &entry)
{
    m_entries.insert(index, entry);
    emit actionAdded(index, entry.action);
}

void QuickMenuContainer::removeAction(int index)
{
    QuickAction *action = m_entries.takeAt(index).action;
    emit actionRemoved(index);
    action->deleteLater();
}

} // namespace qutim_sdk_0_3

#include "quickactionextender.h"

namespace qutim_sdk_0_3 {

QuickActionExtender::QuickActionExtender(QObject *parent) :
    MenuController(parent), m_original(nullptr)
{
}

MenuController *QuickActionExtender::original() const
{
    return m_original;
}

void QuickActionExtender::setOriginal(MenuController *original)
{
    if (m_original == original)
        return;

    setMenuOwner(original);

    m_original = original;
    emit originalChanged(original);
}

QQmlListProperty<QObject> QuickActionExtender::actions()
{
    QQmlListProperty<QObject> actions;
    return actions;
}

} // namespace qutim_sdk_0_3

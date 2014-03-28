#include "quickpluginwrapper.h"
#include <qutim/plugin.h>

namespace qutim_sdk_0_3 {

QuickPluginWrapper::QuickPluginWrapper(QObject *parent) :
    QObject(parent)
{
}

QString QuickPluginWrapper::name() const
{
    return m_name;
}

void QuickPluginWrapper::setName(const QString &name)
{
    if (m_name != name) {
        m_name = name;
        m_object.clear();

        for (QPointer<Plugin> plugin : pluginsList()) {
            if (plugin && plugin->info().libraryName() == name) {
                m_object = plugin;
                break;
            }
        }

        emit nameChanged(name);
        emit objectChanged(m_object);
    }
}

QObject *QuickPluginWrapper::object() const
{
    return m_object;
}

} // namespace qutim_sdk_0_3

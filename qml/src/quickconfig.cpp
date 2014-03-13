#include "quickconfig.h"

namespace qutim_sdk_0_3 {

QuickConfig::QuickConfig(QObject *parent) :
    QObject(parent)
{
}

void QuickConfig::setPath(const QString &path)
{
    if (m_path != path) {
        m_path = path;
        m_config = Config(path);
        
        if (!m_group.isEmpty())
            m_config.beginGroup(m_group);
        
        emit pathChanged();
    }
}

QString QuickConfig::path() const
{
    return m_path;
}

void QuickConfig::setGroup(const QString &group)
{
    if (m_group != group) {
        if (!m_group.isEmpty())
            m_config.endGroup();
        
        m_group = group;
        if (!group.isEmpty())
            m_config.beginGroup(group);
    }
}

QString QuickConfig::group() const
{
    return m_group;
}

QVariant QuickConfig::value(const QString &name, const QVariant &defaultValue)
{
    return m_config.value(name, defaultValue);
}

void QuickConfig::setValue(const QString &name, const QVariant &value)
{
    m_config.setValue(name, value);
}

void QuickConfig::beginGroup(const QString &name)
{
    m_config.beginGroup(name);
}

void QuickConfig::endGroup()
{
    m_config.endGroup();
}

} // namespace qutim_sdk_0_3


void qutim_sdk_0_3::QuickConfig::classBegin()
{
}

void qutim_sdk_0_3::QuickConfig::componentComplete()
{
}

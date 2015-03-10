#include "quickconfig.h"
#include <qutim/account.h>
#include <qutim/protocol.h>

namespace qutim_sdk_0_3 {

QuickConfig::QuickConfig(QObject *parent) :
    QObject(parent)
{
	setObjectName(QStringLiteral("QuickConfig"));
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

QObject *QuickConfig::object() const
{
    return m_object;
}

void QuickConfig::setObject(QObject *object)
{
    if (m_object != object) {
        m_object = object;

        if (Account *account = qobject_cast<Account *>(m_object))
            m_config = account->config();
        else if (Protocol *protocol = qobject_cast<Protocol *>(m_object))
            m_config = protocol->config();
        else
            m_config = Config(m_path);

        if (!m_group.isEmpty())
            m_config.beginGroup(m_group);

        emit objectChanged(object);
    }
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

void QuickConfig::forceSync()
{
	m_config.sync();
}

void QuickConfig::classBegin()
{
}

void QuickConfig::componentComplete()
{
}

} // namespace qutim_sdk_0_3
